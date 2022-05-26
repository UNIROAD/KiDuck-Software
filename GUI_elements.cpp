#include <string>
#include <vector>

using namespace std;

class UI_element{
protected:
    int id;
    vector<int> size;
};

class Screen{
protected:
    int id;
    vector<UI_element> elements;
    vector< vector <int> > compiled_screen;
public:
    bool compile();
};

class button : public UI_element {
protected:
    char* name;
    bool pressed;

public:
    button(int id, vector<int> size, char* name){
        this->id = id;
        this->size = size;
        this->name = name;
        this->pressed = false;
    }

    bool toggle(){
        this->pressed = ~this->pressed;
        return this->pressed;
    }
};

class textBox : public UI_element {
protected:
    char* text;
    bool pressed;
public:
    textBox(int id, vector<int> size){
        this->id = id;
        this->size = size;
        this->text = "";
    }
    bool addText(char add);
    bool deleteOne();
    bool deleteAll();
};


class keyboard : public UI_element {
protected:
    char* chars;
    int length;
    int current_idx;
public:
    keyboard(int id, vector<int> size, char* chars){
        this->id = id;
        this->size = size;
        this->chars = chars;
        this->length = sizeof(chars)/sizeof(char);
        this->current_idx = 0;
    }

    bool moveCursor(bool direction);
    bool select();
};
