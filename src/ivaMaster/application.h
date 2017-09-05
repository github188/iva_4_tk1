#ifndef APPLICATION_H
#define APPLICATION_H

class Application
{
public:
    Application(){m_iMasterMqid = -1;};
    virtual ~Application(){};
	
	void init();
	
    int exec();

	void quit(int returnCode);
private:
	
	int m_iMasterMqid;
};

#endif // APPLICATION_H
