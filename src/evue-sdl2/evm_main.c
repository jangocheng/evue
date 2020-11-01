#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "evm.h"
#include "evm_main.h"
#include "lvgl.h"
#include "lvgl_main.h"
#include "ecma.h"
#include "../frameworks/js_app.h"

/*****************REPL*******************/
// 定义REPL接口函数evm_repl_tty_read，从tty终端获取字符
#ifdef EVM_LANG_ENABLE_REPL
#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#endif

#ifdef __WIN64__
#include <conio.h>
#endif

#ifdef __linux__
/**
 * @brief linux平台终端repl读取单个字符接口
 * @return 单个字符
 */
char mygetch(void)  // 不回显获取字符
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    newt.c_cc[VEOL] = 1;
    newt.c_cc[VEOF] = 2;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

#ifdef __WIN64__
/**
 * @brief windows平台终端repl读取单个字符接口
 * @return 单个字符
 */
char mygetch(void)  // 不回显获取字符
{
    return getch();
}

#endif


// 如果启动REPL, 必须实现evm_repl_tty_read接口，
/**
 * @brief 终端repl读取单个字符接口, 如果是单片机实时调试，基于串口读取字符实现
 * @param e, 虚拟机对象
 * @return 单个字符
 */
char evm_repl_tty_read(evm_t * e)
{
    EVM_UNUSED(e);
    return mygetch();
}
#endif

int modules_paths_count = 1;
char* modules_paths[] = {
    "./",
};

enum FS_MODE{
    FS_READ = 1,
    FS_WRITE = 2,
    FS_APPEND = 4,
    FS_CREATE = 8,
    FS_OPEN = 16,
    FS_TEXT = 32,
    FS_BIN = 64,
};

void * fs_open(char * name, int mode)
{
    char m[5];
    memset(m, 0, 5);
    if( mode & FS_READ)
        sprintf(m, "%sr", m);

    if( mode & FS_WRITE)
        sprintf(m, "%sw", m);

    if( mode & FS_TEXT)
        sprintf(m, "%st", m);

    if( mode & FS_BIN)
        sprintf(m, "%sb", m);

    if( mode & FS_APPEND)
        sprintf(m, "%sa", m);

    if( mode & FS_TEXT)
        sprintf(m, "%st", m);

    return fopen(name, m);
}

void fs_close(void * handle)
{
    fclose((FILE*)handle);
}

int fs_size(void * handle)
{
    FILE *file = (void*)handle;
    fseek (file , 0 , SEEK_END);
    int lSize = ftell (file);
    rewind (file);
    return lSize;
}

int fs_read(void * handle, char * buf, int len)
{
    return fread (buf, 1, len, (FILE*)handle);
}

int fs_write(void * handle, char * buf, int len)
{
    return fwrite(buf, 1, len, (FILE*)handle);
}

char * loadconfig(char *filename){
    FILE *file;
    size_t result;
    uint32_t lSize;
    char *buffer = NULL;

    file = fs_open(filename, FS_READ | FS_TEXT);
    if (file == NULL) return NULL;
    lSize = fs_size(file);
    buffer = (char*)malloc(lSize+1);;
    memset(buffer, 0, lSize + 1);
    result = fs_read(file, buffer, lSize);
    if (!result){
        fclose(file);
        return NULL;
    }
    buffer[lSize] = 0;
    fs_close(file);
    return buffer;
}


char * vm_load_file(evm_t * e, char *filename){
    FILE *file;
    size_t result;
    uint32_t lSize;
    char *buffer = NULL;

    file = fs_open(filename, FS_READ | FS_TEXT);
    if (file == NULL) return NULL;
    lSize = fs_size(file);
    evm_val_t * b = evm_buffer_create(e, sizeof(uint8_t)*lSize + 1);
    buffer = (char*)evm_buffer_addr(b);
    memset(buffer, 0, lSize + 1);
    result = fs_read(file, buffer, lSize);
    if (!result){
        fclose(file);
        return NULL;
    }
    buffer[lSize] = 0;
    fclose(file);
    return buffer;
}

const char * vm_load(evm_t * e, char * path, int type)
{
    int file_name_len = strlen(path) + 1;
    char* buffer = NULL;
    if(type == EVM_LOAD_MAIN){
        char * module_name = evm_malloc(file_name_len);
        if( !module_name ) return NULL;
        sprintf(module_name, "%s", path);
        sprintf(e->file_name, "%s", path);
        buffer = vm_load_file(e, module_name);
        evm_free(module_name);
    } else {
        for(int i=0; i< modules_paths_count; i++){
            int len = strlen(modules_paths[i]) + 1 + file_name_len;
            char* modules_path = evm_malloc(len);
            sprintf(modules_path,  "%s/%s", modules_paths[i], path);
            sprintf(e->file_name, "%s", path);
            buffer = vm_load_file(e, modules_path);
            evm_free(modules_path);
            if (buffer){
                break;
            }
        }
    }
    return buffer;
}


void * vm_malloc(int size)
{
    void * m = malloc(size);
    if(m) memset(m, 0 ,size);
    return m;
}

void vm_free(void * mem)
{
    if(mem) free(mem);
}


static char router_app_dir[PATH_LENGTH_MAX];

int evm_main(char * appdir)
{
    if (appdir == NULL){
        sprintf(router_app_dir, "%s", ".");
    }else{
        sprintf(router_app_dir, "%s", appdir);
    }

    lvgl_main();

    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);

    evm_t * env = js_app_on_create(router_app_dir);

    while(1){
        usleep(1000);
        evm_lvgl_tick_inc(1);
        lv_task_handler();
        ecma_timeout_poll(env);
    }
    return 0;
}
