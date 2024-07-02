#include <iostream>
#include <vector>
#include <string>
#include <cmath>
using namespace std;

const int SIZE = 9;
const int maxK = 1000;
const int SIZE_SQUARED = SIZE * SIZE;
const int SQRT_SIZE = sqrt(SIZE);
const int ROWS = SIZE * SIZE * SIZE;
const int COLS = 4 * SIZE * SIZE;

class node {
public:
    int size;
    node* left, * right, * up, * down, * head;
    int mapper[3]; // number,row,col
};

node Head;
node* headNode = &Head;
vector<node*> solMatrix(maxK, nullptr);
vector<node*> origValues(maxK, nullptr);
vector<vector<bool>> matrix(ROWS, vector<bool>(COLS, 0));
bool flag = false;

void coverCol(node* col) {
    col->left->right = col->right;
    col->right->left = col->left;
    for (node* nd = col->down; nd != col; nd = nd->down) {
        for (node* temp = nd->right; temp != nd; temp = temp->right) {
            temp->down->up = temp->up;
            temp->up->down = temp->down;
            temp->head->size--;
        }
    }
}

void uncoverCol(node* col) {
    for (node* nd = col->up; nd != col; nd = nd->up) {
        for (node* temp = nd->left; temp != nd; temp = temp->left) {
            temp->head->size++;
            temp->down->up = temp;
            temp->up->down = temp;
        }
    }
    col->left->right = col;
    col->right->left = col;
}

void createMatrix() {
    // Rule 1
    int col = 0, ct = 0;
    for (int i = 0; i < ROWS; i++) {
        matrix[i][col] = 1;
        ct++;
        if (ct >= SIZE) {
            ct = 0;
            col++;
        }
    }

    // Rule 2
    int x = 0; ct = 1;
    for (int j = SIZE_SQUARED; j < 2 * SIZE_SQUARED; j++) {
        for (int i = x; i < ct * SIZE_SQUARED; i += SIZE) {
            matrix[i][j] = 1;
        }
        if ((j + 1) % SIZE == 0) {
            x = ct * SIZE_SQUARED;  // for changing row(of sudoku)
            ct++;
        }
        else x++; // for changing digit
    }

    // Rule 3
    int j = 2 * SIZE_SQUARED;
    for (int i = 0; i < ROWS; i++) {
        matrix[i][j] = 1;
        j++;
        if (j >= 3 * SIZE_SQUARED) j = 2 * SIZE_SQUARED;
    }

    // Rule 4
    x = 0;
    for (j = 3 * SIZE_SQUARED; j < COLS; j++) {
        for (int i = 0; i < SQRT_SIZE; i++) {
            for (int k = 0; k < SQRT_SIZE; k++) {
                matrix[x + i * SIZE + k * SIZE_SQUARED][j] = 1;
            }
        }

        int tmp = j + 1 - 3 * SIZE_SQUARED;
        if (tmp % (SQRT_SIZE * SIZE) == 0) x += (SQRT_SIZE - 1) * SIZE_SQUARED + (SQRT_SIZE - 1) * SIZE + 1;
        else if (tmp % SIZE == 0) x += SIZE * (SQRT_SIZE - 1) + 1;
        else x++;
    }
}

void createLinks() {
    node* h = new node;
    h->left = h;
    h->right = h;
    h->up = h;
    h->down = h;
    h->head = h;
    h->size = -1;
    node* tmp = h;

    for (int i = 0; i < COLS; i++) {
        node* curr = new node;
        curr->size = 0;
        curr->left = tmp;
        curr->right = h;
        curr->up = curr;
        curr->down = curr;
        curr->head = curr;
        tmp->right = curr;
        tmp = curr;
    }

    int id[3] = { 0,1,1 };
    for (int i = 0; i < ROWS; i++) {
        node* top = h->right;
        node* prev = nullptr;

        if (i != 0 && i % SIZE_SQUARED == 0) {
            id[0] -= SIZE - 1;
            id[1]++;
            id[2] -= SIZE - 1;
        }
        else if (i != 0 && i % SIZE == 0) {
            id[0] -= SIZE - 1;
            id[2]++;
        }
        else {
            id[0]++;
        }

        for (int j = 0; j < COLS; j++) {
            if (matrix[i][j]) {
                node* curr = new node;
                curr->mapper[0] = id[0];
                curr->mapper[1] = id[1];
                curr->mapper[2] = id[2];
                if (prev == nullptr) {
                    prev = curr;
                    prev->right = curr;
                }
                curr->left = prev;
                curr->right = prev->right;
                curr->right->left = curr;
                prev->right = curr;
                curr->head = top;
                curr->down = top;
                curr->up = top->up;
                top->up->down = curr;
                top->size++;
                top->up = curr;

                if (top->down == top) top->down = curr;
                prev = curr;
            }
            top = top->right;
        }
    }
    headNode = h;
}

void listToMatrix(vector<vector<int>>& sudoku) {
    int idx = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (sudoku[i][j] > 0) {
                node* Col = nullptr;
                node* temp = nullptr;
                for (Col = headNode->right; Col != headNode; Col = Col->right) {
                    for (temp = Col->down; temp != Col; temp = temp->down)
                        if (temp->mapper[0] == sudoku[i][j] && (temp->mapper[1] - 1) == i && (temp->mapper[2] - 1) == j)
                            goto Devansha;
                }
Devansha:
                coverCol(Col);
                origValues[idx] = temp;
                idx++;
                for (node* nd = temp->right; nd != temp; nd = nd->right) {
                    coverCol(nd->head);
                }
            }
}

void solToMatrix(vector<vector<int>>& sudoku) {
    for (int i = 0; solMatrix[i] != nullptr; i++) {
        sudoku[solMatrix[i]->mapper[1] - 1][solMatrix[i]->mapper[2] - 1] = solMatrix[i]->mapper[0];
    }
    for (int i = 0; origValues[i] != nullptr; i++) {
        sudoku[origValues[i]->mapper[1] - 1][origValues[i]->mapper[2] - 1] = origValues[i]->mapper[0];
    }
}

void printSol(vector<vector<int>>& sudoku) {
    cout<<"\nThe solved Sudoku puzzle is as follows: \n\n";
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cout << sudoku[i][j] << " ";
        }
        cout << endl;
    }
}

void search(int k) {
    if (headNode->right == headNode) {
        vector<vector<int>> grid(SIZE, vector<int>(SIZE, 0));
        solToMatrix(grid);
        printSol(grid);
        exit(0);
        std::cin.get(); //Pause console
        flag = true;
        return;
    
    }

    //Choose Column Object Deterministically: Choose the column with the smallest Size
    node* Col = headNode->right;
    for (node* temp = Col->right; temp != headNode; temp = temp->right)
        if (temp->size < Col->size)
            Col = temp;

    coverCol(Col);

    for (node* temp = Col->down; temp != Col; temp = temp->down) {
        solMatrix[k] = temp;
        for (node* nd = temp->right; nd != temp; nd = nd->right) {
            coverCol(nd->head);
        }

        search(k + 1);

        temp = solMatrix[k];
        solMatrix[k] = nullptr;
        Col = temp->head;
        for (node* nd = temp->left; nd != temp; nd = nd->left) {
            uncoverCol(nd->head);
        }
    }

    uncoverCol(Col);
}

void solve(vector<vector<int>>& sudoku) {
    createMatrix();
    createLinks();
    listToMatrix(sudoku);
    search(0);
    if (!flag) {
        cout << "No Solution exits for the given puzzle! Try Again:(\n";
    }
}

int main() {
    
    vector<vector<int>> sudoku(SIZE, vector<int>(SIZE, 0));
    cout<<"Please Enter the 9x9 sudoku puzzle, in the form of a sudoku matrix."<<endl;
    cout<<"Enter 0 for a missing number."<<endl;
    
    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++) cin>>sudoku[i][j];
    }
    vector<vector<int>> puzzle = {
        { 0,0,0,  0,0,0,  0,0,0 },
        { 0,0,0,  0,0,3,  0,8,5 },
        { 0,0,1,  0,2,0,  0,0,0 },
        { 0,0,0,  5,0,7,  0,0,0 },
        { 0,0,4,  0,0,0,  1,0,0 },
        { 0,9,0,  0,0,0,  0,0,0 },
        { 5,0,0,  0,0,0,  0,7,3 },
        { 0,0,2,  0,1,0,  0,0,0 },
        { 0,0,0,  0,4,0,  0,0,9 }
    };
    solve(sudoku);
    std::cin.get();
    return 0;
}
