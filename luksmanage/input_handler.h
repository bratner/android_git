#ifndef INPUT_HANDLER_H

int init_input(void (*on_key)(int keycode), void (*on_click)(int x, int y), void (*on_touch)(int x, int y, int touch));
int kill_input();

#endif
