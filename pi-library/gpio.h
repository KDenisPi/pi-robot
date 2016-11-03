namespace gpio {

class Gpio
{
  public:
    Gpio();
    ~Gpio();

   static int UNDEFINED;

   const int getPin() const {return m_pin;};
   void setPin(int pin); 

   const int getMode() const {return m_mode;};
   void setMode(int mode);

  private:
   int m_pin;
   int m_mode; //INPUT/OUTPUT
   
};

}
