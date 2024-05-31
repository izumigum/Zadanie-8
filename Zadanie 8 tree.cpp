#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Node {
private:
    string name;
    string value;
    vector<Node*> children;
    Node* predok;
    string* attrs_name;
    string* attrs_value;
    int countAttr;

public:
    Node() : name("123"), value("321\t"), predok(nullptr), countAttr(0) {}
    Node(string name) : name(name), value(""), predok(nullptr), countAttr(0) {}

    void setName(string name) {
        this->name = name;
    }

    string getName() {
        return name;
    }

    void setValue(string value) {
        this->value = value;
    }

    void setValue(double value) {
        this->value = to_string(value);
    }

    void setValue(int value) {
        this->value = to_string(value);
    }

    string getValue() {
        return value;
    }

    bool isInt() {
        try {
            stoi(value);
            return true;
        }
        catch (...) {
            return false;
        }
    }

    bool isDouble() {
        try {
            stod(value);
            return true;
        }
        catch (...) {
            return false;
        }
    }

    int getInt() {
        try {
            return stoi(value);
        }
        catch (...) {
            return 0;
        }
    }

    double getDouble() {
        try {
            return stod(value);
        }
        catch (...) {
            return 0;
        }
    }

    Node* getPredok() {
        return predok;
    }

    void setPredok(Node* predok) {
        this->predok = predok;
    }

    int getCountSub() {
        return children.size();
    }

    Node* getSub(int index) {
        if (index < 0 || index >= children.size())
            return nullptr;
        return children[index];
    }

    Node* getSub(string name) {
        for (auto child : children) {
            if (child->getName() == name)
                return child;
        }
        return nullptr;
    }

    void addSub(Node* child) {
        children.push_back(child);
        child->setPredok(this);
    }

    void deleteSub(int index) {
        if (index < 0 || index >= children.size())
            return;
        children.erase(children.begin() + index);
    }

    vector<Node*> getChildren() {
        return children;
    }

    string getAttrName(int index) {
        return attrs_name[index];
    }

    string getAttrValue(int index) {
        return attrs_value[index];
    }

    void setAttrName(int index, string new_name) {
        if (index >= 0 && index < countAttr) attrs_name[index] = new_name;
    }

    void setAttrValue(int index, string new_value) {
        if (index >= 0 && index < countAttr) attrs_value[index] = new_value;
    }

    int getCountAttrs() {
        return countAttr;
    }

    void addAttr(string newAttr, string value) {
        string* new_attrs = new string[countAttr + 1];
        string* new_attrs_value = new string[countAttr + 1];
        for (int i = 0; i < countAttr; i++) {
            new_attrs[i] = attrs_name[i];
            new_attrs_value[i] = attrs_value[i];
        }
        new_attrs[countAttr] = newAttr;
        new_attrs_value[countAttr] = value;
        attrs_name = new_attrs;
        attrs_value = new_attrs_value;
        countAttr++;
    }
};

class MultiplTree {
private:
    Node* root;
    Node* current;

    vector<string> split(const string& str, char delimiter) {
        vector<string> tokens;
        string token;
        istringstream tokenStream(str);
        while (getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    Node* navigateToPath(const string& path) {
        vector<string> tokens = split(path, '/');
        Node* node = current;
        for (const string& token : tokens) {
            if (token == "..") {
                if (node->getPredok() != nullptr) {
                    node = node->getPredok();
                }
                else {
                    return nullptr;  // нет родительского узла, путь неверный
                }
            }
            else {
                node = node->getSub(token);
                if (node == nullptr) {
                    return nullptr;  // не удалось перейти в указанный узел
                }
            }
        }
        return node;
    }

public:
    MultiplTree() : root(new Node("root")), current(root) {}

    void load(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Не удалось открыть файл " << filename << endl;
            return;
        }

        string line;
        Node* parentNode = current;
        while (getline(file, line)) {
            vector<string> tokens = split(line, ' ');
            if (tokens.size() > 0) {
                Node* newNode = new Node(tokens[0]);
                if (tokens.size() > 1) {
                    newNode->setValue(tokens[1]);
                }
                parentNode->addSub(newNode);
                parentNode = newNode;
            }
        }
        file.close();
    }

    void add(const string& path) {
        size_t pos = path.find_last_of('/');
        string parentPath = (pos == string::npos) ? "" : path.substr(0, pos);
        string nodeName = (pos == string::npos) ? path : path.substr(pos + 1);

        Node* parentNode = (parentPath.empty()) ? current : navigateToPath(parentPath);
        if (parentNode == nullptr) {
            cerr << "Не удалось найти путь " << parentPath << endl;
            return;
        }

        Node* newNode = new Node(nodeName);
        parentNode->addSub(newNode);
    }

    void del(const string& path) {
        size_t pos = path.find_last_of('/');
        string parentPath = (pos == string::npos) ? "" : path.substr(0, pos);
        string nodeName = (pos == string::npos) ? path : path.substr(pos + 1);

        Node* parentNode = (parentPath.empty()) ? current : navigateToPath(parentPath);
        if (parentNode == nullptr) {
            cerr << "Не удалось найти путь " << parentPath << endl;
            return;
        }

        vector<Node*> children = parentNode->getChildren();
        for (int i = 0; i < children.size(); i++) {
            if (children[i]->getName() == nodeName) {
                parentNode->deleteSub(i);
                return;
            }
        }
        cerr << "Не удалось найти узел " << nodeName << endl;
    }

    void ls() {
        vector<Node*> children = current->getChildren();
        for (Node* child : children) {
            cout << child->getName() << endl;
        }
    }

    void cd(const string& path) {
        Node* node = navigateToPath(path);
        if (node != nullptr) {
            current = node;
        }
        else {
            cerr << "Не удалось перейти в узел " << path << endl;
        }
    }

    void edit(const string& path, const string& newName, const string& newValue) {
        Node* node = navigateToPath(path);
        if (node != nullptr) {
            node->setName(newName);
            node->setValue(newValue);
        }
        else {
            cerr << "Не удалось найти узел " << path << endl;
        }
    }
};

class Controller {
private:
    MultiplTree mainTree;
public:
    void execute() {
        string name;
        cout << "Введите название файла для загрузки: "; cin >> name;
        mainTree.load(name);
        bool run = true;

        while (run) {
            showMenu();
            int k = -1;
            while (k < 1 || k > 7) {
                cout << "Выберите вариант: ";
                cin >> k;
            }
            if (k == 1) {
                mainTree.ls();
            }
            if (k == 2) {
                string name;
                cout << "Введите имя узла, в который хотите перейти: "; cin >> name;
                mainTree.cd(name);
            }
            if (k == 3) {
                cout << "Введите путь узла, который хотите отредактировать: ";
                string path, newName, newValue;
                cin >> path;
                cout << "Введите новое имя узла: "; cin >> newName;
                cout << "Введите новое значение узла: "; cin >> newValue;
                mainTree.edit(path, newName, newValue);
            }
            if (k == 4) {
                string name;
                cout << "Введите имя узла, который хотите удалить: "; cin >> name;
                mainTree.del(name);
            }
            if (k == 5) {
                string name;
                cout << "Введите имя узла, который хотите добавить: "; cin >> name;
                mainTree.add(name);
            }
            if (k == 6) {
                cout << "Сохранение файла пока не реализовано" << endl;
            }
            if (k == 7) {
                run = false;
            }
        }
    }

    void showMenu() {
        cout << "Выберите команду из списка" << endl;
        cout << "1. ls" << endl;
        cout << "2. cd" << endl;
        cout << "3. edit" << endl;
        cout << "4. delete" << endl;
        cout << "5. add" << endl;
        cout << "6. save_file" << endl;
        cout << "7. exit" << endl;
    }
};

int main() {
    setlocale(LC_ALL, "RU");
    Controller ctrl;
    ctrl.execute();
    return 0;
}