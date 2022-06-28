#include <string>
#include <vector>

using namespace std;

class UI_element{
protected:
    int width, height;
    int x_pos, y_pos;
public:
    void draw();
};

// class Div{
// protected:
//     vector<UI_element> elements;
// public:
//     void draw();
// };


#define DIV_WIDTH_DIRECTION true
#define DIV_HEIGHT_DIRECTION false

class Div{
    
/*
    Div class calculates the position of a divided section of a screen
    if given a size of the screen and desired number of divisions
    It can also account for padding between sections
*/

protected:
    int width, height;
    int width_div, height_div;
    int padding;
    
public:
    int sect_width, sect_height;
    Div(int width, int height, int width_div, int height_div, int padding){
        this->width = width;
        this->height = height;
        this->width_div = width_div;
        this->height_div = height_div;
        this->padding = padding;

        this->sect_width = (width-padding) / width_div - padding;
        this->sect_height = (height-padding) / height_div - padding;
    }

    // position of a section
    int position(int num, bool direction){
        // if direction: width, elif !direction: height
        int size = (direction)?this->width:this->height;
        int div = (direction)?this->width_div:this->height_div;
        return this->padding + (size - this->padding) * (num + ((num<0)?div:0)) / div;
    }

    // position of a text in center of a section
    // int text_size : text width or height in pixels
    int text_center_pos(int text_size, int num, bool direction){
        // if direction: width, elif !direction: height
        int sect_size = (direction)?this->sect_width:this->sect_height;
        return position(num, direction) + (sect_size - text_size) / 2;
    }
};

class TextDiv: public Div{

};

class Screen{
protected:
    vector<Div> divs;
    
public:
    Screen(){
        
    }
    void draw(){
        
    }
    void controls(){

    }
};


class Texts : public UI_element{
protected:
    string text;
public:
    Texts(int width, int height, int x_pos, int y_pos, string text){
        this->width = width;
        this->height = height;
        this->x_pos = x_pos;
        this->y_pos = y_pos;
        this->text = text;
    }
};

class ToggleButton : public UI_element {
protected:
    string name;
    bool pressed;

public:
    ToggleButton(int width, int height, int x_pos, int y_pos, string name){
        this->width = width;
        this->height = height;
        this->x_pos = x_pos;
        this->y_pos = y_pos;
        this->name = name;
        this->pressed = false;
    }

    bool toggle(){
        this->pressed = !this->pressed;
        return this->pressed;
    }
};

class TextBox : public UI_element {
protected:
    string text;
    bool pressed;
public:
    TextBox(int width, int height, int x_pos, int y_pos){
        this->width = width;
        this->height = height;
        this->x_pos = x_pos;
        this->y_pos = y_pos;
        this->text = "";
    }
    void addText(char add) {this->text.push_back(add);}
    void deleteOne() {this->text.pop_back();}
    void deleteAll() {this->text = "";}
};

class List : public UI_element {
protected:
    int length;     // length of the list
    int visible_len; // number of list visible on screen

    // values that indicates the current position state of the list
    int cursor_pos; // position of the cursor on list of visible screen : 0 < this < visible_len
    int list_pos;   // list number which is on the top of the screen
    int curr;       // current position of the cursor on the whole list

    string *texts;  // list of text on the list
    void (**actions)(); // list of functions --------------> to be implemented
public:
    List(int width, int height, int x_pos, int y_pos, int length, int visible_len, string* texts, void (**actions)()){
        this->width = width;
        this->height = height;
        this->x_pos = x_pos;
        this->y_pos = y_pos;

        this->length = length;
        this->visible_len = visible_len;
        this->cursor_pos = 0;
        this->list_pos = 0;
        this->curr = 0;

        this->texts = texts;
        this->actions = actions;
    }

    int moveBackward(){
        // moves cursor if cursor isn't at the top of the screen
        if(this->cursor_pos>0){
            this->cursor_pos--;
            return --(this->curr);
        }
        // moves list if cursor is at the top of the screen
        else if(this->list_pos>0){
            this->list_pos--;
            return --(this->curr);
        }
        return 0;
    }

    int moveForward(){
        // moves cursor if cursor isn't at the bottom of the screen
        if(this->cursor_pos<this->visible_len-1){
            this->cursor_pos++;
            return ++(this->curr);
        }
        // moves list if cursor is at the bottom of the list
        else if(this->list_pos<this->length-this->visible_len){
            this->list_pos++;
            return ++(this->curr);
        }
        return this->length-1;
    }

    // does some action when list element selected
    void select() {this->actions[this->curr]();}
};
