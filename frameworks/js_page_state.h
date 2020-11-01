/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JS_PAGE_STATE_H
#define JS_PAGE_STATE_H

#include "js_page_state_machine.h"

void js_page_state_handle_init_state(js_page_sm_t * sm);
void js_page_state_handle_ready_state(js_page_sm_t * sm);
void js_page_state_handle_show_state(js_page_sm_t * sm);
void js_page_state_handle_background_state(js_page_sm_t * sm);
void js_page_state_handle_destroy_state(js_page_sm_t * sm);


#endif
