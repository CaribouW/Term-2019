#include <iostream>
class A
{
    int x, y;

public:
    virtual void f()
    {
        using namespace std;
        cout << "A f" << endl;
    }
    virtual void g()
    {
        using namespace std;
        cout << "A g" << endl;
    }
    void h()
    {
        using namespace std;
        cout << "A h" << endl;
    }
};

class B : public A
{
public:
    void f() override
    {
        using namespace std;
        cout << "B f" << endl;
    }
    void h()
    {
        using namespace std;
        cout << "B h" << endl;
    }
};

int main()
{
    A a;
    B b;
    A *p;
    p = &b;
    p->h();
}