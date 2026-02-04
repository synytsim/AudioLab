#include "Wave.h"

Node::Node(void) {
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

Operand::Operand(void) {
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

Node& Add::copy(void) const {
    return *(new Add(&(this->op1->copy()), &(this->op2->copy())));
}

Node& Mul::copy(void) const {
    return *(new Mul(&(this->op1->copy()), &(this->op2->copy())));
}

Node& Operand::copy(void) const {
    return *(new Operand(this->wave));
}

float Mul::getValue(void) const {
    return this->op1->getValue() * this->op2->getValue();
}

float Add::getValue(void) const {
    return this->op1->getValue() + this->op2->getValue();
}

float Operand::getValue(void) const {
    return this->wave->getWaveValue();
}

void Operand::set(const ClassWave& aWave) {
    this->wave = (ClassWave *)&aWave;
}

NodeType Add::getType(void) const {
    return NodeType::Add;
}

NodeType Mul::getType(void) const {
    return NodeType::Mul;
}

NodeType Operand::getType(void) const {
    return NodeType::Operand;
}