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

    ClassWave(void);
    ClassWave(float frequency, float amplitude, float phase);

    ~ClassWave();  

    void setAll(float frequency, float amplitude, float phase);
    void resetAll(void);

    void setFrequency(float frequency);
    void setAmplitude(float amplitude);
    void setPhase(float phase);

    float getFrequency(void) const;
    float getAmplitude(void) const;
    float getPhase(void) const;

    // pure virtual function for getting the value associated with the wave
    virtual float getWaveValue(void) const = 0;

    // calculate values for a 1Hz sine wave sampled at SAMPLE_RATE
    static void calculateSineWave(void);

    static void iterateTimeIndex(void);

    static void synchronizeTimeIndex(void);

    Add& operator+(const ClassWave& right);
    Mul& operator*(const ClassWave& right);

    Add& operator+(const Node& right);
    Mul& operator*(const Node& right);

    Add& operator+(const Composite& right);
    Mul& operator*(const Composite& right);

};

class DC: public ClassWave {
  public:
    DC(void);
    DC(float amplitude);

    void setAll(float frequency, float amplitude, float phase);

    void setFrequency(float frequency) = delete;
    void setPhase(float phase) = delete;

    float getFrequency(void) = delete;
    float getPhase(void) = delete;

    float getWaveValue(void) const;
};

class Sine: public ClassWave
{
  public:
    Sine(void);
    Sine(float frequency, float amplitude, float phase = 0);
    float getWaveValue(void) const;
};

class Cosine: public ClassWave
{
  public:
    Cosine(void);
    Cosine(float frequency, float amplitude, float phase = 0);
    float getWaveValue(void) const;
};

class Square: public ClassWave
{
    private:
        float duty_cycle;
  public:
    Square(void);
    Square(float frequency, float amplitude, float phase = 0, float duty_cycle = 0.5);
    void setDutyCycle(float duty_cycle);
    float getDutyCycle(void) const;
    float getWaveValue(void) const;
};

class Sawtooth: public ClassWave
{
  public:
    Sawtooth(void);
    Sawtooth(float frequency, float amplitude, float phase = 0);
    float getWaveValue(void) const;
};

class Triangle: public ClassWave
{
  public:
    Triangle(void);
    Triangle(float frequency, float amplitude, float phase = 0);
    float getWaveValue(void) const;
};

enum class NodeType { Node, Add, Mul, Operand, Composite };

class Node {
    private:
        friend class ClassWave;

        // static Operand operands[MAX_NUM_NODES];
        // static Add adds[MAX_NUM_NODES];
        // static Mul muls[MAX_NUM_NODES];

    protected:
        Node *op1;
        Node *op2;
    public:
        Node(void);
        ~Node();
        virtual float getValue(void) const = 0;
        virtual Node& copy(void) const = 0;
        virtual NodeType getType(void) const = 0;

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
        float getValue(void) const override;
        Node& copy(void) const override;
        NodeType getType(void) const override;
};

class Mul : public Node {
    public:
        Mul(Node *opA, Node *opB);
        float getValue(void) const override;
        Node& copy(void) const override;
        NodeType getType(void) const override;
};

class Operand : public Node {
    private:
        ClassWave *wave;
    public:
        Operand(void);
        Operand(ClassWave *aWave);
        float getValue(void) const override;
        Node& copy(void) const override;
        NodeType getType(void) const override;
        void set(const ClassWave& aWave);
};

class Composite : public Node {
    private:
        Composite(Node* node);
    public:
        Composite(void);
        Composite(const Node &op1);
        Composite(const ClassWave &op1);

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

        float getValue(void) const override;
        Node& copy(void) const override;
        NodeType getType(void) const override;
};

#endif