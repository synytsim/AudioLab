#include "Node.h"

Node::Node() {
    this->op1 = nullptr;
    this->op2 = nullptr;
}

Node::~Node() {
    if (this->op1) delete this->op1;
    this->op1 = nullptr;
    if (this->op2) delete this->op2;
    this->op2 = nullptr;
}

Add::Add(Node *opA, Node *opB) { 
    this->op1 = opA;
    this->op2 = opB;
}

Mul::Mul(Node *opA, Node *opB) { 
    this->op1 = opA;
    this->op2 = opB;
}

Operand::Operand() {
    this->wave = 0;
}

Operand::Operand(ClassWave *aWave) {
    this->wave = aWave;
}

Add& Node::operator+(const ClassWave& right) {
    Operand* operand = new Operand((ClassWave *)&right);
    return *(new Add(this, operand));
}

Mul& Node::operator*(const ClassWave& right) {
    Operand* operand = new Operand((ClassWave *)&right);
    return *(new Mul(this, operand));
}

Add& Node::operator+(const Node& right) {
    return *(new Add(this, (Node *)&right));
}

Mul& Node::operator*(const Node& right) {
    return *(new Mul(this, (Node *)&right));
}

Add& Node::operator+(const Composite& right) {
    return *(new Add(this, &right.op1->copy()));
}

Mul& Node::operator*(const Composite& right) {
    return *(new Mul(this, &right.op1->copy()));
}

Node& Add::copy() {
    return *(new Add(&(this->op1->copy()), &(this->op2->copy())));
}

Node& Mul::copy() {
    return *(new Mul(&(this->op1->copy()), &(this->op2->copy())));
}

Node& Operand::copy() {
    return *(new Operand(this->wave));
}

float Mul::getValue() {
    return this->op1->getValue() * this->op2->getValue();
}

float Add::getValue() {
    return this->op1->getValue() + this->op2->getValue();
}

float Operand::getValue() {
    return this->wave->getWaveValue();
}

void Operand::set(const ClassWave& aWave) {
    this->wave = (ClassWave *)&aWave;
}