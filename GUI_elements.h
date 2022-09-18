#include <string>
#include <math.h>

using namespace std;

class UI_element{
protected:
    int width, height;
    int x_pos, y_pos;
public:
    UI_element(int width, int height, int x_pos, int y_pos){
        this->width = width;
        this->height = height;
        this->x_pos = x_pos;
        this->y_pos = y_pos;
    }
    // getters
    int getWidth(){return this->width;}
    int getHeight(){return this->height;}
    int getXPos(){return this->x_pos;}
    int getYPos(){return this->y_pos;}
    void draw();
};


#define DIV_WIDTH_DIRECTION true
#define DIV_HEIGHT_DIRECTION false
#define DIV_LEFT_ALLIGNMENT -1
#define DIV_CENTER_ALLIGNMENT 0
#define DIV_RIGHT_ALLIGNEMENT 1
#define DIV_PADDED true
#define DIV_PADLESS false

class Div: public UI_element{
    
/*
    Div class calculates the position of a divided section of a screen
    if given a size of the screen and desired number of divisions
    It can also account for padding between sections
*/

protected:
    int width_div, height_div;
    int padding;
    
public:
    int sect_width, sect_height;
    Div(int width, int height, int x_pos, int y_pos, int width_div, int height_div, int padding)
    : UI_element(width, height, x_pos, y_pos){
        this->width_div = width_div;
        this->height_div = height_div;
        this->padding = padding;

        this->sect_width = (width-padding) / width_div - padding;
        this->sect_height = (height-padding) / height_div - padding;
    }

    // getters
    int getSectWidth(){return this->sect_width;}
    int getSectHeight(){return this->sect_height;}

    // position of a section
    int position(int num, bool direction, bool padded){
        // if direction: width, elif !direction: height
        int size = (direction)?this->width:this->height;
        int pos = (direction)?this->x_pos:this->y_pos;
        int div = (direction)?this->width_div:this->height_div;
        if(padded == DIV_PADLESS) return pos + size/div * num;
        return pos + this->padding + (size - this->padding) * (num + ((num<0)?div:0)) / div;
    }

    // width or height of multiple sections
    int multiSectSize(int len, bool direction, bool padded){
        // if direction: width, elif !direction: height
        int sect_size = (direction)?this->sect_width:this->sect_height;
        return (sect_size+this->padding) * len + this->padding * ((padded)?1:-1);
    }

    // position of a text in left/center/right allignment of a section
    // int text_size : text width or height in pixels
    int textAllign(int text_size, int num, int allign, bool direction){
        // if direction: width, elif !direction: height
        int sect_size = (direction)?this->sect_width:this->sect_height;
        
        // left allignment
        if(allign == DIV_LEFT_ALLIGNMENT) return position(num, direction, DIV_PADDED);
        // center allignment
        else if(allign == DIV_CENTER_ALLIGNMENT) return position(num, direction, DIV_PADDED) + (sect_size - text_size) / 2;
        // right allignment
        else if(allign == DIV_RIGHT_ALLIGNEMENT) position(num, direction, DIV_PADDED) + text_size - sect_size;
        return 0;
    }
};

class TextDiv: public Div{

};

class Screen{
    virtual void draw() = 0;
    virtual void controls() = 0;
    virtual void buttonMap() = 0;
};


class Texts : public UI_element{
protected:
    string text;
public:
    Texts(int width, int height, int x_pos, int y_pos, string text)
    : UI_element(width, height, x_pos, y_pos){
        this->text = text;
    }
};

class ToggleButton : public UI_element {
protected:
    string name;
    bool pressed;

public:
    ToggleButton(int width, int height, int x_pos, int y_pos, string name)
    : UI_element(width, height, x_pos, y_pos){
        this->name = name;
        this->pressed = false;
    }

    bool toggle(){
        this->pressed = !this->pressed;
        return this->pressed;
    }
};


class List : public UI_element {
protected:
    int length;     // length of the list
    int visible_len; // number of list visible on screen

    // values that indicates the current position state of the list
    int cursor_pos; // position of the cursor on list of visible screen : 0 < this < visible_len
    int list_pos;   // list number which is on the top of the screen
    int curr;       // current position of the cursor on the whole list

    string **texts;  // list of text on the list
public:
    List(int width, int height, int x_pos, int y_pos, int length, int visible_len, string** texts)//}, void (**actions)()){
    : UI_element(width, height, x_pos, y_pos){ 
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
    }

    //getters
    int getVisibleLen(){return this->visible_len;}
    int getCursorPos(){return this->cursor_pos;}
    int getListPos(){return this->list_pos;}
    int getCurr(){return this->curr;}

    string getVisibleText(int idxdiff){return *(this->texts[this->list_pos + idxdiff]);}
    void moveBackward(){
        // moves cursor if cursor isn't at the top of the screen
        if(this->cursor_pos>0){
            this->cursor_pos--;
            this->curr--;
        }
        // moves list if cursor is at the top of the screen
        else if(this->list_pos>0){
            this->list_pos--;
            this->curr--;
        }
    }

    void moveForward(){
        // moves cursor if cursor isn't at the bottom of the screen
        if(this->cursor_pos<this->visible_len-1){
            this->cursor_pos++;
            this->curr++;
        }
        // moves list if cursor is at the bottom of the list
        else if(this->list_pos<this->length-this->visible_len){
            this->list_pos++;
            this->curr++;
        }
    }
};

class Textbox : public UI_element {
protected:
    string text;
    int length;
public:
    Textbox(int width, int height, int x_pos, int y_pos, int length)
    : UI_element(width, height, x_pos, y_pos){
        this->text = "";
        this->length = length;
    }
    string getText(){return this->text;}
    
    void addText(char add) {this->text.push_back(add);}
    void deleteOne() {this->text.pop_back();}
    void deleteAll() {this->text = "";}
    string flush() {
        string temp = this->text;
        this->text = "";
        return temp;
    }
};

class Keyboard : public UI_element {
protected:
    int length;     // length of the list
    int visible_len; // number of list visible on screen
    int curr;       // current position of the cursor on the whole list
    char *chrs;  // list of text on the keyboard
    
public:
    Keyboard(int width, int height, int x_pos, int y_pos, int length, int visible_len, char* chrs)
    : UI_element(width, height, x_pos, y_pos){
        this->length = length;
        this->visible_len = visible_len;    // odd number
        this->curr = 0;
        this->chrs = chrs;
    }

    //getters
    int getVisibleLen(){return this->visible_len;}

    char getVisibleText(int idxdiff){
        if(0<=(this->curr + idxdiff)&&(this->curr + idxdiff)<this->length){
            return this->chrs[this->curr + idxdiff];
        }
        return ' ';
    }

    void moveBackward(){if(0<this->curr) this->curr--;}
    void moveForward(){if(this->curr<this->length-2) this->curr++;}

    void enter(Textbox* textbox){(*textbox).addText(this->getVisibleText(0));}
};

class Slider : public UI_element {
protected:
    int length;     // length of the list
    int curr;       // current position of the cursor on the whole list
    
public:
    Slider(int width, int height, int x_pos, int y_pos, int length)
    : UI_element(width, height, x_pos, y_pos){
        this->length = length;
        this->curr = 0;
    }

    int getCurr(){return this->curr;}

    void moveBackward(){if(0<this->curr) this->curr--;}
    void moveForward(){if(this->curr<this->length-1) this->curr++;}

};
