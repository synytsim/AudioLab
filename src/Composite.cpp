#include "Node.h"

Composite::Composite() {
    this->op1 = 0;
}

Composite::~Composite() {

}

Composite::Composite(Node *op) {
    this->op1 = op;
}

Composite::Composite(const Node &op) {
    this->op1 = (Node *)&op;
}

Composite::Composite(const ClassWave& op) {
    if (this->op1) delete op1;
    this->op1 = new Operand((ClassWave *)&op);
}

Composite& Composite::operator=(const ClassWave& right) {
    if (this->op1) delete op1;
    this->op1 = new Operand((ClassWave *)&right);
    return *this;
}

Composite& Composite::operator=(const Composite& right) {
    if (this->op1) delete op1;
    this->op1 = &right.op1->copy();
    return *this;
}

Composite& Composite::operator=(const Node& right) {
    if (this->op1) delete op1;
    this->op1 = (Node *)&right;
    return *this;
}

Add& Composite::operator+(const Node& right) {
    return *this->op1 + right;
}

Mul& Composite::operator*(const Node& right) {
    return *this->op1 * right;
}

Add& Composite::operator+(const ClassWave& right) {
    return this->op1->copy() + right;
}

Mul& Composite::operator*(const ClassWave& right) {
    return this->op1->copy() * right;
}

Add& Composite::operator+(const Composite& right) {
    return *(new Add(&this->copy(), &right.op1->copy()));
}

Mul& Composite::operator*(const Composite& right) {
    return *(new Mul(&this->copy(), &right.op1->copy()));
}

Composite& Composite::operator+=(const Node& right) {
    this->op1 = &(*this->op1 + right);
    return *this;
}

Composite& Composite::operator*=(const Node& right) {
    this->op1 = &(*this->op1 * right);
    return *this;
}

Composite& Composite::operator+=(const ClassWave& right) {
    this->op1 = &(*this->op1 + right);
    return *this; 
}

Composite& Composite::operator*=(const ClassWave& right) {
    this->op1 = &(*this->op1 * right);
    return *this; 
}

Node& Composite::copy() {
    return this->op1->copy();
}

float Composite::getValue() {
    return this->op1->getValue();
}