#include <iostream>
#include <fstream>
using namespace std;
pthread_mutex_t mx=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

struct word
{
    string code;
    char ch;
    string newword;
};

void *removechar(void *wrd)
{
    pthread_mutex_lock(&mx);
    //Start of Critical Section
    //cout<<"locked"<<endl;
    struct word *wrds=(struct word *)wrd;
    char ch=wrds->ch;
    string code=wrds->code;
    string newword=wrds->newword;
    string temp="";
    if(int(ch)==10)
            cout<<"<EOL> Binary code = "<<code<<endl;
    else
        cout<<ch<<" Binary code = "<<code<<endl;
    int count;
    for(int i=0;i<code.length();i++)
    {
        if(code[i]=='1')
        {
            temp+=ch;
        }
        else
        {
            temp+=newword[count];
            count++;
        }
    }
    wrds->newword=temp;
    //End of Critical Section
    pthread_mutex_unlock(&mx);
    //cout<<"unlocked"<<endl<<endl;
    pthread_cond_signal(&cond);
    return 0;
}

int main()
{
    string input;
    char c;
    int numeol=0;
    while(cin.get(c))
    {
        input+=c;
        if(int(c)==10)
            numeol++;
    }
    numeol++;
    char carr[numeol];
    string sarr[numeol];
    string in[numeol];
    int count=0;
    string temp;
    string s;
    for(int i=input.length()-1;i>=0;i--)
    {   
        if((input[i]=='0'||input[i]=='1')&&i!=0&&int(input[i-1])!=10)
        {
            temp+=input[i];
        }
        else if(int(input[i])!=32)
        {
            if(int(input[i])==62)
            {
                carr[count]=char(10);
                sarr[count]=temp;
                count++;
                temp="";
            }
            else if(int(input[i])!=10)
            {
                carr[count]=input[i];
                sarr[count]=temp;
                if(temp!="")
                    count++;
                temp="";
            }
        }
        else if(int(input[i])==32)
        {
            if(int(input[i+1])==32)
            {
                carr[count]=char(32);
                sarr[count]=temp;
                if(temp!="")
                    count++;
                temp="";
            }
        }
    }
    //cout<<count<<endl;
    for(int j=0;j<count;j++)
    {
        char flipc=carr[j];
        string flips=sarr[j];
        int n = flips.length();
        for (int i = 0; i < n / 2; i++) 
            swap(flips[i], flips[n - i - 1]); 
        carr[j]=flipc;
        sarr[j]=flips; 
        //cout<<flipc<<" "<<flips<<endl;
    }
    pthread_t tid[count];
    pthread_mutex_init(&mx, NULL);
    struct word w;
    for(int i=0;i<count;i++)
    {
        w.ch=carr[i];
        w.code=sarr[i];
        if(pthread_create(&tid[i], NULL, removechar, &w)) 
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
        pthread_cond_wait(&cond,&mx);
    }
    for (int i = 0; i < count; i++)
        pthread_join(tid[i], NULL);
    cout<<"Decompressed file contents:"<<endl;
    cout<<w.newword<<endl; 
    return 0;
}
