#ifndef NODE_H
#define NODE_H

class Add;
class Mul;
class Composite;
class ClassWave;

class Node {
    private:
        friend class ClassWave;
    protected:
        Node *op1;
        Node *op2;
    public:
        Node();
        ~Node();
        virtual float getValue() = 0;
        virtual Node& copy() = 0;

        Add& operator+(const ClassWave& right);
        Mul& operator*(const ClassWave& right);
        
        Add& operator+(const Node& right);
        Mul& operator*(const Node& right);

        Add& operator+(const Composite& right);
        Mul& operator*(const Composite& right);
};

class Add : public Node {
    public:
        Add(Node *opA, Node *opB);
        float getValue() override;
        Node& copy() override;
};

class Mul : public Node {
    public:
        Mul(Node *opA, Node *opB);
        float getValue() override;
        Node& copy() override;
};

class Operand : public Node {
    private:
        ClassWave *wave;
    public:
        Operand();
        Operand(ClassWave *aWave);
        float getValue() override;
        Node& copy() override;
        void set(const ClassWave& aWave);
};

class Composite : public Node {
    private:
        Composite(Node* node);
    public:
        Composite();
        Composite(const Node &op1);
        Composite(const ClassWave &op1);
        ~Composite();

        Composite& operator=(const ClassWave& right);
        Composite& operator=(const Node& right);
        Composite& operator=(const Composite& right);

        Add& operator+(const Node& right);
        Mul& operator*(const Node& right);

        Add& operator+(const ClassWave& right);
        Mul& operator*(const ClassWave& right);

        Add& operator+(const Composite& right);
        Mul& operator*(const Composite& right);

        Composite& operator+=(const Node& right);
        Composite& operator*=(const Node& right);
        
        Composite& operator*=(const ClassWave& right);
        Composite& operator+=(const ClassWave& right);

        Composite& operator+=(const Composite& right);
        Composite& operator*=(const Composite& right);

        float getValue() override;
        Node& copy();

};

#endif