#include<iostream>
#include<string>
using namespace std;

int main(){

	int bits[8];
	for(int i=0; i<6; i++)
	{
		bits[i] = 0;
	}
	
	bits[7] = bits[6] = 1;
	bool finished = false;

	do{
		string thisVP;
		for(int i=0; i<8; i++)
		{
			if(bits[i] == 1)
			{
				char c = 'A' + i;
				thisVP += c;
			}
		}

		if(thisVP.size() > 1)
		{
		    if(thisVP.find("A") != -1)
		    {
		        if(thisVP.find("H") != -1)
		        {
		            cout << thisVP << endl;
		        }
		    }
		    if(thisVP.find("B") != -1)  // 2 * 2^4 = 32
		    {
		        if(thisVP.find("G") != -1)
		        {
		            int count = 0;
		            if(thisVP.find("A") != -1) // BGAnotH or BGHnot A
		            {
		                count += 1;
		            }
		            if(thisVP.find("H") != -1)
		            {
		                count += 1;
		            }
		            if (count == 1)
		            {
		                cout << thisVP << endl;
		            }
		        }
		    }
		}
		int j=7;
		while(j>=0 && bits[j] == 1){
			bits[j] = 0;
			j--;
		}

		if(j>=0)
		{
			bits[j] = 1;
		}
		else
		{
			finished = true;
		}

	}while(!finished);


	return 0;
}
