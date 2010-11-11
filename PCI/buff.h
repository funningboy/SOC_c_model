
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define FREQ1	400
#define FREQ2	200
#define BUFFDEP 5	


using namespace std;


enum BUFF_ST {
	BUF_EMPTY	=0,
	BUF_FULL	=1,
	BUF_RD_OT	=2,
	BUF_WT_OT	=3,
	BUF_RD_OK	=4,
	BUF_WT_OK	=5,
};

template<class T>
class BUFF_CSS {

public :
	BUFF_CSS(int i) : index(i) {}
	~BUFF_CSS(){}

 	int 	SET2READ(T r);	
	int	SET2WRITE(T &w);
	void	DisPlayBUFF();
private :
	T	Buff[BUFFDEP];
	int	index;
};
