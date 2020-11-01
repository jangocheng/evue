/**
 * @file lv_marquee.h
 *
 */

#ifndef LV_MARQUEE_H
#define LV_MARQUEE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "src/lv_widgets/lv_label.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a label objects
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_marquee_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_marquee_set_text(lv_obj_t * label, const char * text);

/**
 * Set a new formatted text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param fmt `printf`-like format
 */
void lv_marquee_set_text_fmt(lv_obj_t * label, const char * fmt, ...);

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void lv_marquee_set_text_static(lv_obj_t * label, const char * text);

/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'lv_marquee_long_mode' enum.
 *                  In LV_LONG_BREAK/LONG/ROLL the size of the label should be set AFTER this
 * function
 */
void lv_marquee_set_long_mode(lv_obj_t * label, lv_label_long_mode_t long_mode);

/**
 * Set the align of the label (left or center)
 * @param label pointer to a label object
 * @param align 'LV_LABEL_ALIGN_LEFT' or 'LV_LABEL_ALIGN_LEFT'
 */
void lv_marquee_set_align(lv_obj_t * label, lv_label_align_t align);

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param en true: enable recoloring, false: disable
 */
void lv_marquee_set_recolor(lv_obj_t * label, bool en);

/**
 * Set the label's animation speed in LV_LABEL_LONG_SROLL/SCROLL_CIRC modes
 * @param label pointer to a label object
 * @param anim_speed speed of animation in px/sec unit
 */
void lv_marquee_set_anim_speed(lv_obj_t * label, uint16_t anim_speed);

/**
 * @brief Set the selection start index.
 * @param label pointer to a label object.
 * @param index index to set. `LV_LABEL_TXT_SEL_OFF` to select nothing.
 */
void lv_marquee_set_text_sel_start(lv_obj_t * label, uint32_t index);

/**
 * @brief Set the selection end index.
 * @param label pointer to a label object.
 * @param index index to set. `LV_LABEL_TXT_SEL_OFF` to select nothing.
 */
void lv_marquee_set_text_sel_end(lv_obj_t * label, uint32_t index);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
char * lv_marquee_get_text(const lv_obj_t * label);

/**
 * Get the text size of a label
 * @param label pointer to a label object
 * @return the size of the label
 */
lv_point_t lv_marquee_get_text_size(lv_obj_t * label);

/**
 * Get the long mode of a label
 * @param label pointer to a label object
 * @return the long mode
 */
lv_label_long_mode_t lv_marquee_get_long_mode(const lv_obj_t * label);

/**
 * Get the align attribute
 * @param label pointer to a label object
 * @return LV_LABEL_ALIGN_LEFT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_marquee_get_align(const lv_obj_t * label);

/**
 * Get the recoloring attribute
 * @param label pointer to a label object
 * @return true: recoloring is enabled, false: disable
 */
bool lv_marquee_get_recolor(const lv_obj_t * label);

/**
 * Get the label's animation speed in LV_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @return speed of animation in px/sec unit
 */
uint16_t lv_marquee_get_anim_speed(const lv_obj_t * label);

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter [0 ... text length]. Expressed in character index, not byte
 * index (different in UTF-8)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void lv_marquee_get_letter_pos(const lv_obj_t * label, uint32_t index, lv_point_t * pos);

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 * Expressed in character index and not byte index (different in UTF-8)
 */
uint32_t lv_marquee_get_letter_on(const lv_obj_t * label, lv_point_t * pos);

/**
 * Check if a character is drawn under a point.
 * @param label Label object
 * @param pos Point to check for character under
 * @return whether a character is drawn under the point
 */
bool lv_marquee_is_char_under_pos(const lv_obj_t * label, lv_point_t * pos);

/**
 * @brief Get the selection start index.
 * @param label pointer to a label object.
 * @return selection start index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_marquee_get_text_sel_start(const lv_obj_t * label);

/**
 * @brief Get the selection end index.
 * @param label pointer to a label object.
 * @return selection end index. `LV_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint32_t lv_marquee_get_text_sel_end(const lv_obj_t * label);


lv_style_list_t * lv_marquee_get_style(lv_obj_t * label, uint8_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Insert a text to the label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char. LV_LABEL_POS_LAST: after last char.
 * @param txt pointer to the text to insert
 */
void lv_marquee_ins_text(lv_obj_t * label, uint32_t pos, const char * txt);

/**
 * Delete characters from a label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char.
 * @param cnt number of characters to cut
 */
void lv_marquee_cut_text(lv_obj_t * label, uint32_t pos, uint32_t cnt);

/**********************
 *      MACROS
 **********************/



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_MARQUEE_H*/
