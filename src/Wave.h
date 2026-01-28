#ifndef Wave_h
#define Wave_h

#include <Arduino.h>
#include <math.h>
#include "AudioLabSettings.h"

class Node;
class Add;
class Mul;
class Composite;

// abstract base class for wave
class ClassWave
{
  protected:

    float frequency;
    float amplitude;
    float phase;
    
    int _phase;

  private:
  
  public:

    ClassWave();

    ~ClassWave();  

    void set(float aFrequency, float anAmplitude, float aPhase);
    void reset();

    void setFrequency(float aFrequency);
    void setAmplitude(float anAmplitude);
    void setPhase(float aPhase);

    float getFrequency() const;
    float getAmplitude() const;
    float getPhase() const;

    // pure virtual function for getting the value associated with the wave
    virtual float getWaveValue() const = 0;

    // calculate values for a 1Hz sine wave sampled at SAMPLE_RATE
    static void calculateSineWave();

    static void iterateTimeIndex();

    static void synchronizeTimeIndex();

    Add& operator+(const ClassWave& right);
    Mul& operator*(const ClassWave& right);

    Add& operator+(const Node& right);
    Mul& operator*(const Node& right);

    Add& operator+(const Composite& right);
    Mul& operator*(const Composite& right);

};

class DC: public ClassWave {
  public:
    DC();
    float getWaveValue() const;
};

class Sine: public ClassWave
{
  public:
    Sine();
    float getWaveValue() const;
};

class Cosine: public ClassWave
{
  public:
    Cosine();
    float getWaveValue() const;
};

class Square: public ClassWave
{
  public:
    Square();
    float getWaveValue() const;
};

class Sawtooth: public ClassWave
{
  public:
    Sawtooth();
    float getWaveValue() const;
};

class Triangle: public ClassWave
{
  public:
    Triangle();
    float getWaveValue() const;
};


enum class NodeType { Node, Add, Mul, Operand, Composite };

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
        virtual NodeType getType() const = 0;

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
        NodeType getType() const override;
};

class Mul : public Node {
    public:
        Mul(Node *opA, Node *opB);
        float getValue() override;
        Node& copy() override;
        NodeType getType() const override;
};

class Operand : public Node {
    private:
        ClassWave *wave;
    public:
        Operand();
        Operand(ClassWave *aWave);
        float getValue() override;
        Node& copy() override;
        NodeType getType() const override;
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
        Node& copy() override;
        NodeType getType() const override;

};

#endif