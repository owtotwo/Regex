#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>
#include <queue>
#include <set>

using std::cout;
using std::endl;


class Edge;

//结点和它指向的其他结点
class Node {
public:
    static int idCount;
    static void resetID() {
        idCount = 0;
    }
    static void addIDCount() {
        idCount++;
    }

    int ID;
    std::vector<Edge> edges;
    Node() {
        ID = idCount;
        cout<<"creat ID = "<<ID<<endl;
        addIDCount();
    }
    //Node(const Node& temp) : ID(temp.ID), nextNodeSet(temp.nextNodeSet) {}
};

class Edge {
public:
    char c;
    Node* pNode;
};

class Graph
{
public:
    Node *start, *end;
    std::vector<Node*> subNode;

    Graph(char c = '\0') {
        start = new Node(), end = new Node();
        Edge midEdge = {c, end};
        cout<<"c = "<<c<<endl;
        start->edges.push_back(midEdge);
        subNode.push_back(start);
        subNode.push_back(end);
    }

    void showGraph() {
        std::queue<Node*> qu;
        std::set<int> s;
        qu.push(start);
        s.insert(start->ID);
        while (!qu.empty()) {
            //cout<<"qu.size() = "<<qu.size()<<endl;
            Node* p = qu.front();
            qu.pop();
            cout << "Node id = " << p->ID << endl;
            for (Edge& x : p->edges) {
                if(find(s.begin(), s.end(), x.pNode->ID) == s.end()) {
                    //cout<<"push x.pNode = " << x.pNode->ID <<endl;
                    qu.push(x.pNode);
                    s.insert(x.pNode->ID);
                }

            }
        }
    }
};

enum countKind { combine = '+', choose = '|', closure = '*'};

Graph combineNFA(Graph a, Graph b) {
    cout << "连接！a->start.id = " << a.start->ID << " b->start.id = " << b.start->ID << endl;
    Edge temp = {'\0', b.start};
    a.end->edges.push_back(temp);
    a.end = b.end;
    return a;
}

Graph chooseNFA(Graph a, Graph b) {
    cout << "选择！a->start.id = " << a.start->ID << " b->start.id = " << b.start->ID << endl;
    Graph temp;
    temp.start->edges.pop_back();
    Edge temp1 = {'\0', a.start};
    Edge temp2 = {'\0', b.start};
    temp.start->edges.push_back(temp1);
    temp.start->edges.push_back(temp2);
    temp1.pNode = temp.end;
    a.end->edges.push_back(temp1);
    b.end->edges.push_back(temp1);
    cout<<"[Show]"<<endl;
    temp.showGraph();
    return temp;
}

Graph closureNFA(Graph a) {
    cout << "闭包！a->start.id = " << a.start->ID << endl;
    Graph temp;
    temp.start->edges[0].pNode = a.start;
    Edge temp1 = {'\0', temp.end};
    a.end->edges.push_back(temp1);
    temp.start->edges.push_back(temp1);
    temp1.pNode = a.start;
    a.end->edges.push_back(temp1);
    return temp;
}

Graph countNFA(char kind, Graph a, Graph b) {
    switch (kind) {
        case '+' :
            return combineNFA(a, b);
        case '|' :
            return chooseNFA(a, b);
        default :
            return NULL;
    }
}

Graph regexToNFA(std::string s) {
    // i.e. (e|f*)a(b|c)*
    std::stack<Graph> NFAstack;
    std::stack<char> opeStack;
    for (int i = 0; i < s.length(); i++) {
        if(!opeStack.empty()) cout<< "stack.top() = " << opeStack.top() << " size of stack = " << opeStack.size() <<endl;
        else cout << "opeStack is empty!" << endl;

        switch (s[i]) {
            case '(' :
                opeStack.push('(');
                break;
            case ')' :
                {
                    char opeTemp1 = opeStack.top();
                    opeStack.pop();
                    while (opeTemp1 != '(' && !opeStack.empty()) {
                        cout << "Come in opeTemp1 = " << opeTemp1 << " size of stack = " << opeStack.size() <<endl;

                        Graph temp1 = NFAstack.top();
                        NFAstack.pop();
                        Graph temp2 = NFAstack.top();
                        NFAstack.pop();
                        NFAstack.push(countNFA(opeTemp1, temp2, temp1));

                        opeTemp1 = opeStack.top();
                        opeStack.pop();
                    }
                }
                break;
            case '|' :
                {
                    char opeTemp2 = opeStack.top();
                    while (!opeStack.empty() && opeTemp2 != '(') {
                        opeStack.pop();
                        Graph temp1 = NFAstack.top();
                        NFAstack.pop();
                        Graph temp2 = NFAstack.top();
                        NFAstack.pop();
                        NFAstack.push(countNFA(opeTemp2, temp2, temp1));
                        opeTemp2 = opeStack.top();
                    }
                    opeStack.push('|');
                }
                break;
            case '*' :
                {
                    Graph temp = NFAstack.top();
                    NFAstack.pop();
                    NFAstack.push(closureNFA(temp));
                }
                break;
            case '+' :
                {
                    Graph temp1 = NFAstack.top();
                    NFAstack.pop();
                    Graph temp2 = NFAstack.top();
                    NFAstack.pop();
                    NFAstack.push(countNFA(combine, temp2, temp1));
                }
                break;
            default :
                {
                    Graph newGraph = Graph(s[i]);
                    NFAstack.push(newGraph);
                    if (i+1 < s.length() &&
                        s[i+1] != '|' && s[i+1] != '*' && s[i+1] != ')') {
                        opeStack.push('+');
                    }
                }
        }
    }
    while (!opeStack.empty() && !NFAstack.empty()) {
        char opeTemp = opeStack.top();
        opeStack.pop();
        Graph temp1 = NFAstack.top();
        NFAstack.pop();
        Graph temp2 = NFAstack.top();
        NFAstack.pop();
        NFAstack.push(countNFA(opeTemp, temp2, temp1));
    }
    if (!NFAstack.empty()) {
        return NFAstack.top();
    } else {
        return NULL;
    }
}

void showGraph(Graph& temp) {
    std::queue<Node*> qu;
    std::set<int> s;
    qu.push(temp.start);
    while (!qu.empty()) {
        Node* p = qu.front();
        qu.pop();
        cout << "Node id = " << p->ID << endl;
        cout<<"what?"<<endl;
        s.insert(p->ID);
        cout<< " p->edges size() = " << p->edges.size() <<endl;
        for (Edge& x : p->edges) {
            cout<<"进来了"<<endl;
            if(find(s.begin(), s.end(), x.pNode->ID) == s.end()){
                cout<<"没找到"<<endl;
                qu.push(x.pNode);
            }

        }
    }
}


// 读入一个结点的所有边的集合 并返回其所能到的任何结点(包括自身)的集合set
std::set<Node*> epsilonEdge(Node* root) {

    std::vector<Edge>& edges = root->edges; // 根节点

    // 输入的测试是第一个结点ID=1 然后 向后走
    std::set<Node*> reNodeSet;
    std::queue<Node*> qu;

    // 将所有epsilon边指向的结点都加入到qu队列中
    //for (Edge& x: edges) {
    //    if (x.c == '\0')
    //        qu.push(x.pNode);
    //}

    qu.push(root);  //放入根节点到队列中

    cout << "EpsilonEdge() : queue.size() = " << qu.size() << endl;

        // edges 里逐个取出 并进行BFS
        // 每次先探步 看看是否走过( 是否在reNodeSet中
        // 走一步 并记录下点到set中( 并将over置为false
        // 直到没有再向前走了

    while (!qu.empty()) {
        // 每次取一个结点 向后走一步
        Node* tempNode = qu.front();
        qu.pop();
        // 先将自身放入遍历过的集合中
        reNodeSet.insert(tempNode);
        // 取出结点
        for (Edge& x : tempNode->edges) {
            // x为结点的边 逐个边处理
            if (x.c == '\0') {
                Node* temp_node = x.pNode; // 每个边指向的点
                if (reNodeSet.find(temp_node) == reNodeSet.end()) { // 是否已经遍历过
                    reNodeSet.insert(temp_node); // 放入遍历过的集合
                    qu.push(temp_node); // 放入队列
                }
            }
        }
    }

    cout<<"Epsilon reNodeSet = ";
    for (Node* x : reNodeSet) {
        cout << x->ID << " ";
    }
    cout<<endl;
    return reNodeSet;
}

std::string match(std::string s, Graph& graph) {
    // 读入 每一行 判断
    // state set (Node
    std::queue<Node*> stateSet, temp;
    std::set<Node*> tempStartSet = epsilonEdge(graph.start);

    cout << "match() : 开始的结点集合有 = ";
    for (Node* x : tempStartSet) {
        cout << x->ID << " ";
        stateSet.push(x);
    }
    cout << endl;

    bool Over = false;
    std::string restring = "", rets = "";

        // 每次处理一个字符 看走到哪一步 记得判断是否到终结状态
        //
        // 先从stateSet中逐个取出来判断是否有符合的边
        // 若有 则将边指向的结点放进temp队列里
        // 处理完后 stateSet为空
        // temp中逐个取出 自身先加入到stateset中
        // 【注意：每个加入stateSet中的点都要判断是否为终结状态Over】
        // 然后判断epsilon边所能到的结点 直到temp为空
        // 此时stateSet就是下一次循环的初状态
        //
        // 最后 判断stateSet是否为空 为空则判断是否Over
        // 是则break return restring 不是则状态清零 初始化restring为空串
        // 不为空则将字符加入到restring中
        // 出循环后判断是否？

    for(int i = 0; i < s.length(); i++) {

        cout << "stateSet size = " << stateSet.size() << endl;
        cout << "front node = " << stateSet.front()->ID << endl;
        cout << "char c = " << s[i] << endl << endl;

        while (!stateSet.empty()) {
            Node* p = stateSet.front();
            stateSet.pop();
            cout<<"Node ID = " << p->ID << " 's nextNode include ";
            for (Edge& x : p->edges) {
                cout << x.pNode->ID << " ";
                if (x.c == s[i]) {
                    temp.push(x.pNode);
                }
            }
            cout << endl;
        }

        while (!temp.empty()) {
            Node* p = temp.front();
            temp.pop();
            //stateSet.push(p);
            std::set<Node*> tempv = epsilonEdge(p);
            cout << "return epsilonEdge tempv size = " << tempv.size() << endl;
            for (Node* x : tempv) {
                stateSet.push(x);
                cout<<"放入stateSet结点id = "<<x->ID<<endl;
                if (x == graph.end) Over = true;
            }
        }

        if (stateSet.empty()) {
            if (Over) {
                break;
            } else {
                cout << "清空字符" << endl;
                restring = "";
                std::queue<Node*> queueclear_temp;
                stateSet = queueclear_temp;
                stateSet.push(graph.start);
            }
        } else {
            restring += s[i];
            cout << "加入字符" << s[i] << endl;
        }
    }

    //if (Over) return restring;
    //else return restring
    return restring;
}


int Node::idCount = 0;

int main()
{
    Graph temp = regexToNFA("(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)*");
    //Graph temp = regexToNFA("(a|b)*");
    //showGraph(temp);
    temp.showGraph();
    cout<<"尾结点id=="<<temp.end->ID<<endl;
    std::string haha;
    std::cin >> haha ;
    std::string reha = match(haha, temp);
    cout << "最终结果：" << reha << endl;
    return 0;
}
