// typedef timestamp = unsigned long;

// class Task {
//   private:
//     unsigned long period;
// }

// class Ticker {
//   private: 
//     unsigned long period;
//     unsigned long previousStart;
//     void (*trigger)(void);
//     bool enabled;
//   public: 
//     Ticker(unsigned long p, void(*fun)(void)) {
//       period = p;
//       trigger = fun;
//       previousStart = 0;
//     }
//     void check() {
//       unsigned long timeNow = millis();
//       if(timeNow - previousStart > period) {
//         // call function
//         (*trigger)();
//         previousStart = timeNow;
//       }
//     }
//     void disable() {
//       this->enabled = false;
//     }
//     void enable() {
//       this->enabled = true;
//     }
// };