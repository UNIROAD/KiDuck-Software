#include <string>
#include <vector>


// for OLED screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

using namespace std;

class UI_element{
protected:
    vector<int> size;
    vector<int> position;
public:
    void draw();
};

class Div{
protected:
    vector<UI_element> elements;
public:
    void draw();
};

class Screen{
protected:
    vector<Div> divs;
public:
    void draw(){
        
    }
};


class Texts : public UI_element{
protected:
    string text;
public:
    Texts(vector<int> size, vector<int> position, string text){
        this->size = size;
        this->position = position;
        this->text = text;
    }
};

class ToggleButton : public UI_element {
protected:
    string name;
    bool pressed;

public:
    ToggleButton(vector<int> size, vector<int> position, string name){
        this->size = size;
        this->position = position;
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
    TextBox(vector<int> size, vector<int> position){
        this->size = size;
        this->position = position;
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
    List(vector<int> size, vector<int> position, int length, int visible_len, string* texts, void (**actions)()){
        this->size = size;
        this->position = position;

        this->length = length;
        this->visible_len = visible_len;
        this->cursor_pos = 0;
        this->list_pos = 0;
        this->curr = 0;

        this->texts = texts;
        this->actions = actions;
    }

    int moveUp(){
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

    int moveDown(){
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
