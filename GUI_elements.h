#include <math.h>

#ifndef GUIELM

using namespace std;

//########################## Base Classes ##########################//
class UI_element{
protected:
    int width, height;
    int x_pos, y_pos;
public:
    UI_element(int width, int height, int x_pos, int y_pos)
    : width(width), height(height), x_pos(x_pos), y_pos(y_pos){}

    // getters
    int getWidth(){return this->width;}
    int getHeight(){return this->height;}
    int getXPos(){return this->x_pos;}
    int getYPos(){return this->y_pos;}
};


/* Parent Class of List, Keyboard, Slider*/
class Scroller : public UI_element {
protected:
    int length;
    int visible_len;
    int curr;
public:
    Scroller(int width, int height, int x_pos, int y_pos, int length, int visible_len, int curr)
    : UI_element(width, height, x_pos, y_pos), length(length), visible_len(visible_len), curr(curr){}

    int getVisibleLen(){return this->visible_len;}
    int getCurr(){return this->curr;}

    // implement this when inherited
    virtual void moveBackward() = 0;
    virtual void moveForward() = 0;
};

class Screen{
public:
    String title;

    Screen(String title): title(title){}

    void draw(){}
};

//########################## Helper Class ##########################//
#define DIV_DIR_W true
#define DIV_DIR_H false
#define DIV_ALGN_L 0
#define DIV_ALGN_C 1
#define DIV_ALGN_R 2
#define DIV_PAD_O true
#define DIV_PAD_X false

class Div: public UI_element{
    
/*Div class calculates the position of a divided section of a screen
if given a size of the screen and desired number of divisions
    It can also account for padding between sections*/

protected:
    int width_div, height_div;
    int padding;
    
public:
    int sect_width, sect_height;
    Div(int width, int height, int x_pos, int y_pos, int width_div, int height_div, int padding)
    : UI_element(width, height, x_pos, y_pos), width_div(width_div), height_div(height_div), padding(padding){
        this->sect_width  = (width-padding)  / width_div  - padding;
        this->sect_height = (height-padding) / height_div - padding;
    }

    // getters
    int getSectWidth(){return this->sect_width;}
    int getSectHeight(){return this->sect_height;}

    /*position of a section
    if direction: width, elif !direction: height*/
    int pos(int num, bool direction, bool padded){
        int size = (direction)?this->width    :this->height;
        int pos  = (direction)?this->x_pos    :this->y_pos;
        int div  = (direction)?this->width_div:this->height_div;
        if(padded == DIV_PAD_X) return pos + size/div * num;
        return pos + this->padding + (size - this->padding) * (num + ((num<0)?div:0)) / div;
    }

    /*width or height of multiple sections*/
    int multiSectSize(int len, bool direction, bool padded){
        // if direction: width, elif !direction: height
        int sect_size = (direction)?this->sect_width:this->sect_height;
        return (sect_size+this->padding) * len + this->padding * ((padded)?1:-1);
    }

    /*position of a text in left/center/right allignment of a section
    int text_size : text width or height in pixels
    if direction: width, elif !direction: height*/
    int allign(int text_size, int num, int allign, bool direction){
        int sect_size = (direction)?this->sect_width:this->sect_height;
        return pos(num, direction, DIV_PAD_O) + (sect_size - text_size)/2*allign;
    }
};

//########################## UI Element Classes ##########################//
class ToggleButton : public UI_element {
protected:
    String name;
    bool pressed;

public:
    ToggleButton(int width, int height, int x_pos, int y_pos, String name)
    : UI_element(width, height, x_pos, y_pos), name(name), pressed(false){}

    bool toggle(){
        this->pressed = !this->pressed;
        return this->pressed;
    }
};

class List : public Scroller {
protected:
    int cursor_pos; // position of the cursor on list of visible screen : 0 < this < visible_len
    int list_pos;   // list number which is on the top of the screen

    String **texts;  // list of text on the list
public:
    List(int width, int height, int x_pos, int y_pos, int length, int visible_len, String** texts)
    : Scroller(width, height, x_pos, y_pos, length, visible_len, 0), cursor_pos(0), list_pos(0), texts(texts){}

    //getters
    int getCursorPos(){return this->cursor_pos;}
    int getListPos(){return this->list_pos;}
    String getVisibleText(int idxdiff){return *(this->texts[this->list_pos + idxdiff]);}
    Div getDiv(int w_div, int h_div, int pad) {return Div(getWidth(), getHeight(), getXPos(), getYPos(), w_div, h_div, pad);}

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
    String text;
    int length;
public:
    Textbox(int width, int height, int x_pos, int y_pos, int length)
    : UI_element(width, height, x_pos, y_pos), text(""), length(length){}

    String getText(){return this->text;}
    Div getDiv(int w_div, int h_div, int pad) {return Div(getWidth(), getHeight(), getXPos(), getYPos(), w_div, h_div, pad);}

    bool not_empty(){return (bool)this->getText().length();}
    void addText(char add) {this->text.concat(add);}
    void deleteOne() {this->text.remove(this->text.length()-1);}
    void deleteAll() {this->text = "";}
    String flush() {
        String temp = this->text;
        this->deleteAll();
        return temp;
    }
};

class Keyboard : public Scroller {
protected:
    char *chrs;  // list of text on the keyboard
    
public:
    Keyboard(int width, int height, int x_pos, int y_pos, int length, int visible_len, char* chrs)
    :  Scroller(width, height, x_pos, y_pos, length, visible_len, 0), chrs(chrs){}

    //getters
    char getVisibleText(int idxdiff){
        if(0<=(this->curr + idxdiff)&&(this->curr + idxdiff)<this->length){
            return this->chrs[this->curr + idxdiff];
        }
        return ' ';
    }
    Div getDiv(int w_div, int h_div, int pad) {return Div(getWidth(), getHeight(), getXPos(), getYPos(), w_div, h_div, pad);}

    void enter(Textbox* textbox){(*textbox).addText(this->getVisibleText(0));}

    void moveBackward(){if(0<this->curr) this->curr--;}
    void moveForward(){if(this->curr<this->length-2) this->curr++;}

};

class Slider : public Scroller {public:
    Slider(int width, int height, int x_pos, int y_pos, int length)
    :  Scroller(width, height, x_pos, y_pos, length, 0, 0){}

    Div getDiv(int w_div, int h_div, int pad) {return Div(getWidth(), getHeight(), getXPos(), getYPos(), w_div, h_div, pad);}

    void moveBackward(){if(0<this->curr) this->curr--;}
    void moveForward(){if(this->curr<this->length-1) this->curr++;}
};

#define GUIELM
#endif
