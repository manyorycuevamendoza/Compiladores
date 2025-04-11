

//linea 70
class Program{
    private:
        list<Stm*> slist:
    public:
        Program();
        void add(Stm* s);
        void interprete();
        ~Program();

};

Program::Program() {}
Program::~Program() {
    for (auto it = slist.begin(); it != slist.end(); ++it) {
        delete *it;
    }
    slist.clear();
};



