#include "chess_ui.h"

Button **buttons = NULL;
int buttons_count = 0;

TreeNode* new_node(void* control, CONTROL type, int child_num, TreeNode* parent){
	TreeNode *node = malloc(sizeof(TreeNode));
	node->type = type;
	switch (type){
	case (WINDOW) :
		node->control = (Window*)control;
		break;
	case (PANEL) :
		node->control = (Panel*)control;
		break;
	case (LABEL) :
		node->control = (Label*)control;
		break;
	case (BUTTON) :
		node->control = (Button*)control;
		break;
	}
	node->child_num = child_num;
	node->children = NULL;
	if (child_num != 0){
		node->children = malloc(sizeof(TreeNode*) * child_num);
		for (int i = 0; i < child_num; i++) node->children[i] = NULL;
	}
	node->parent = parent;
	if (parent != NULL){
		for (int j = 0; j < parent->child_num; j++){
			if (parent->children[j] == NULL){
				parent->children[j] = node;
				break;
			}
		}
	}
	return node;
}

TreeNode* new_window(char *title, int width, int height, int children){
	SDL_WM_SetCaption(title, title);
	SDL_Surface *w = SDL_SetVideoMode(width, height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (w == NULL) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	/* Clear window to BLACK*/
	if (SDL_FillRect(w, 0, 0) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return 1;
	}
	Window *res = malloc(sizeof(Window));
	res->title = title;
	res->width = width;
	res->height = height;
	res->surface = w;
	return new_node(res, WINDOW, children, NULL);
}

TreeNode* new_panel(TreeNode *parent, char* name, int x, int y, int width, int height, int children, char* file){
	Panel *res = malloc(sizeof(Panel));
	res->img = NULL;
	if (file != NULL) res->img = SDL_LoadBMP(file);
	res->name = name;
	res->x = x;
	res->y = y;
	res->width = width;
	res->height = height;
	return new_node(res, PANEL, children, parent);
}

TreeNode* new_label(TreeNode *parent, char* name, int x, int y, int width, int height, int children, char* file){
	Label *res = malloc(sizeof(Label));
	res->img = NULL;
	if (file != NULL) res->img = SDL_LoadBMP(file);
	res->name = name;
	res->x = x;
	res->y = y;
	res->width = width;
	res->height = height;
	return new_node(res, LABEL, children, parent);
}

TreeNode* new_button(TreeNode *parent, char* name, int x, int y, int width, int height, int children, char* file, void* handler, void* args){
	Button *res = malloc(sizeof(Button));
	res->img = NULL;
	if (file != NULL) res->img = SDL_LoadBMP(file);
	res->name = name;
	res->x = x;
	res->y = y;
	res->width = width;
	res->height = height;
	res->handler = handler;
	res->args = args;
	return new_node(res, BUTTON, children, parent);
}

void add_label_to_button(TreeNode *button, char* pic){
	Button *btn = button->control;
	button->child_num = 1;
	button->children = malloc(sizeof(TreeNode*));
	button->children[0] = NULL;
	new_label(button, pic, btn->x, btn->y, btn->width, btn->height, 0, pic);
}

void remove_label_from_button(TreeNode *button){
	if (button->child_num != 1) return;
	
	Label *lbl = (Label*)button->children[0]->control;
	SDL_FreeSurface(lbl->img);
	free(lbl);
	free(button->children[0]);
	free(button->children);
	button->children = NULL;
	button->child_num = 0;
}

int draw_tree_rec(Window* root, TreeNode* node){
	int x = 0, y = 0, width, height, res = 0;
	SDL_Surface *img = NULL;
	Panel *panel, *parent;
	Label *label;
	Button *button;
	switch (node->type){
	case WINDOW:
		// nothing to do
		break;
	case PANEL:
		panel = (Panel*)node->control;
		if (node->parent->type == PANEL){
			parent = (Panel*)node->parent->control;
			x = parent->x;
			y = parent->y;
		}
		x += panel->x;
		y += panel->y;
		width = panel->width;
		height = panel->height;
		img = panel->img;
		break;
	case LABEL:
		label = (Label*)node->control;
		TreeNode *parent_n = node->parent;
		while (parent_n->type != PANEL) parent_n = parent_n->parent;
		parent = (Panel*)parent_n->control;
		x = parent->x + label->x;
		y = parent->y + label->y;
		width = label->width;
		height = label->height;
		img = label->img;
		break;
	case BUTTON:
		button = (Button*)node->control;
		parent = (Panel*)node->parent->control;
		x = parent->x + button->x;
		y = parent->y + button->y;
		button->abs_x = x;
		button->abs_y = y;
		width = button->width;
		height = button->height;
		img = button->img;
		break;
	default:
		printf("ERROR: failed to draw the screen %s, unknown controller or wrong hierarchy in the UI tree", root->title);
		return 1;
	}

	if (node->type != WINDOW && img != NULL){
		SDL_Rect dest = { x, y, width, height }, src = { 0, 0, width, height };
		if (SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 48, 48, 48)) != 0) {
			printf("ERROR: failed to set color key: %s\n", SDL_GetError());
			SDL_FreeSurface(img);
			return 1;
		}
		SDL_BlitSurface(img, &src, root->surface, &dest);
	}

	for (int i = 0; i < node->child_num; i++) res += draw_tree_rec(root, node->children[i]);
	return res;
}

int draw_tree(TreeNode* root){
	Window *win = (Window*)root->control;
	win->surface = SDL_SetVideoMode(win->width, win->height, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	int chk = draw_tree_rec(win, root);
	if (!chk) SDL_Flip(win->surface);
	else return 1;
}

void free_tree(TreeNode *node){
	for (int i = 0; i < node->child_num; i++){
		free_tree(node->children[i]);
	}
	if (node->type != WINDOW){
		Panel *p = (Panel*)node->control;
		if (p->img != NULL){
			SDL_FreeSurface(p->img);
			p->img = NULL;
		}
	}
	if (node->type == BUTTON){
		Button *b = (Button*)node->control;
	}
	free(node->control);
	if (node->children != NULL) free(node->children);
	free(node);
	node = NULL;
}

void get_screen_buttons(TreeNode *node){
	for (int i = 0; i < node->child_num; i++) get_screen_buttons(node->children[i]);
	if (node->type == BUTTON) buttons[buttons_count++] = (Button*)node->control;
}


int is_click_on_button(int x, int y, Button *button){
	return (x > button->abs_x && x < (button->abs_x + button->width) && y > button->abs_y && y < (button->abs_y + button->height));
}

void run_events_loop(TreeNode* screen){
	buttons = malloc(sizeof(Button*) * 100); // max num of buttons per panel
	get_screen_buttons(screen);
	realloc(buttons, sizeof(Button*) * buttons_count);
	SDL_Event e;
	int stop = 0;
	while (!stop){
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit_all();
				stop = 1;
				glob_quit = 1;
				game_on = 0;
			}
			else if (e.type == SDL_MOUSEBUTTONUP){
				int x, y;
				SDL_GetMouseState(&x, &y);
				for (int i = 0; i < buttons_count; i++)
					if (is_click_on_button(x, y, buttons[i])){
					if (buttons[i]->handler)
						buttons[i]->handler(buttons[i]->args);
					stop = 1;
					break;
					}
			}
		}
		SDL_Delay(1);
	}
	free(buttons);
	buttons = NULL;
	buttons_count = 0;
}