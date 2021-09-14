/**************************************************************************************
 * ��������_public.h���������ܺ���������ͷ�ļ�
 *
 * ���ܣ�TcpServer�ࡢTcpClient�ࡢCFile�ļ���д�ࡢCLogFile��־�ļ��ࡢ
 *       ��ӡ����ϵͳʱ�䡢XML�ַ���������HTTP��������Ӧ����
 *************************************************************************************/
#include "_cmpublic.h"

#define COLOR(msg, code) "\033[0;" #code "m" msg "\033[0m"
#define RED(msg)     COLOR(msg, 31)
#define GREEN(msg)   COLOR(msg, 32)
#define YELLOW(msg)  COLOR(msg, 33)
#define BLUE(msg)    COLOR(msg, 34)

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: http/0.1.0\r\n"

/*************************************************************************************/
/* ������socketͨ�ŵĺ������� */

// �ͻ��˷�װ
class CTcpClient
{
public:
  int m_sockfd;       //�ͻ��˵�sockfd
  char m_ip[21];      //����˵�ip
  int m_port;         //�����˵�ͨ�Ŷ˿�
  bool m_btimeout;    //����Read��Write����ʱ��ʧ�ܵ�ԭ���Ƿ��ǳ�ʱ��
                      //true-δ��ʱ��false-�ѳ�ʱ��

  int m_buflen;       //����Read�����󣬽��յ��ı��ĵĴ�С����λ���ֽڡ�

  CTcpClient();       //���캯��

  // �����˷�����������
  // ip������˵�ip
  // port������˼����Ķ˿�
  // ����ֵ���ɹ�����true��ʧ�ܷ���false
  bool ConnectToServer(const char *ip,const int port);

  // ���շ���˷��͹��������ݡ�
  // buffer���������ݻ������ĵ�ַ�����ݵĳ��ȴ����m_buflen��Ա�����С�
  // itimeout���ȴ����ݵĳ�ʱʱ�䣬��λ���룬ȱʡֵ��0-���޵ȴ���
  // ����ֵ��true-�ɹ���false-ʧ�ܣ�ʧ�������������1���ȴ���ʱ����Ա����m_btimeout��ֵ
  //������Ϊtrue��2��socket�����Ѳ����á�
  bool Read(char *buffer,const int itimeout);

  // �����˷������ݡ�
  // buffer�����������ݻ������ĵ�ַ��
  // ibuflen�����������ݵĴ�С����λ���ֽڣ�ȱʡֵΪ0��������͵���ascii�ַ�����ibuflenȡ0������Ƕ����������ݣ�ibuflenΪ���������ݿ�Ĵ�С��
  // ����ֵ��true-�ɹ���false-ʧ�ܣ����ʧ�ܣ���ʾsocket�����Ѳ����á�
  bool Write(const char *buffer,const int ibuflen=0);

  //�Ͽ������˵�����
  void Close();
 
  //�����������Զ��ر�sockfd���ͷ���Դ
  ~CTcpClient();

};

//��C++�ķ�����װsocket�����
class CTcpServer
{
private:
  int m_socklen;                     // �ṹ��struct sockaddr_in�Ĵ�С��
  struct sockaddr_in m_clientaddr;   // �ͻ��˵ĵ�ַ��Ϣ
  struct sockaddr_in m_servaddr;     // ����˵ĵ�ַ��Ϣ

public:
  int m_listenfd;                    // ��������ڼ�����socket
  int m_connfd;                      // �ͻ�����������socket
  bool m_btimeout;                   // ����Read��Write����ʱ��ʧ�ܵ�ԭ���Ƿ��ǳ�ʱ��tr                                     //    ue-δ��ʱ��false-�ѳ�ʱ��
  int m_buflen;                      // ����Read�����󣬽��յ��ı��ĵĴ�С����λ���ֽ�
  
 
  CTcpServer();                      // ���캯��
 
  // ����˳�ʼ����
  // port��ָ����������ڼ����Ķ˿ڡ�
  // ����ֵ��true-�ɹ���false-ʧ�ܣ�һ������£�ֻҪport������ȷ��û�б�ռ�ã���ʼ����>��ɹ���
  bool InitServer(const unsigned int port);


  // �����ȴ��ͻ��˵���������
  // ����ֵ��true-���µĿͻ���������������false-ʧ�ܣ�Accept���жϣ����Acceptʧ�ܣ���>������Accept��         
  bool Accept();  // �ȴ��ͻ��˵�����
 
  // ��ȡ�ͻ��˵�ip��ַ��
  // ����ֵ���ͻ��˵�ip��ַ����"192.168.1.100"��
  char *GetIP();

  // ���տͻ��˷��͹��������ݡ�
  // buffer���������ݻ������ĵ�ַ�����ݵĳ��ȴ����m_buflen��Ա�����С�
  // itimeout���ȴ����ݵĳ�ʱʱ�䣬��λ���룬ȱʡֵ��0-���޵ȴ���
  // ����ֵ��true-�ɹ���false-ʧ�ܣ�ʧ�������������1���ȴ���ʱ����Ա����m_btimeout��ֵ
  // ������Ϊtrue��2��socket�����Ѳ����á�
  bool Read(char *buffer,const int itimeout);

  // ��ͻ��˷������ݡ�
  // buffer�����������ݻ������ĵ�ַ��
  // ibuflen�����������ݵĴ�С����λ���ֽڣ�ȱʡֵΪ0��������͵���ascii�ַ�����ibuflen  //ȡ0������Ƕ����������ݣ�ibuflenΪ���������ݿ�Ĵ�С��
  // ����ֵ��true-�ɹ���false-ʧ�ܣ����ʧ�ܣ���ʾsocket�����Ѳ����á�
  bool Write(const char *buffer,const int ibuflen=0);

  // �رռ�����socket����m_listenfd�������ڶ���̷��������ӽ��̴�����
  void CloseListen();

  // �رտͻ��˵�socket����m_connfd�������ڶ���̷������ĸ����̴����С�
  void CloseClient();

  ~CTcpServer();  // ���������Զ��ر�socket���ͷ���Դ��
};

// ����socket�ĶԶ˷��͹��������ݡ�
// sockfd�����õ�socket���ӡ�
// buffer���������ݻ������ĵ�ַ��
// ibuflen�����γɹ��������ݵ��ֽ�����
// itimeout�����յȴ���ʱ��ʱ�䣬��λ���룬ȱʡֵ��0-���޵ȴ���
// ����ֵ��true-�ɹ���false-ʧ�ܣ�ʧ�������������1���ȴ���ʱ��2��socket�����Ѳ����á�
bool TcpRead(const int sockfd,char *buffer,int *ibuflen,const int itimeout=0);

// ��socket�ĶԶ˷������ݡ�
// sockfd�����õ�socket���ӡ�
// buffer�����������ݻ������ĵ�ַ��
// ibuflen�����������ݵ��ֽ�����������͵���ascii�ַ�����ibuflenȡ0������Ƕ����������ݣ�ibuflenΪ���������ݿ�Ĵ�С��
// ����ֵ��true-�ɹ���false-ʧ�ܣ����ʧ�ܣ���ʾsocket�����Ѳ����á�
bool TcpWrite(const int sockfd,const char *buffer,const int ibuflen=0);

// ���Ѿ�׼���õ�socket�ж�ȡ���ݡ�
// sockfd���Ѿ�׼���õ�socket���ӡ�
// buffer���������ݻ������ĵ�ַ��
// n�����ν������ݵ��ֽ�����
// ����ֵ���ɹ����յ�n�ֽڵ����ݺ󷵻�true��socket���Ӳ����÷���false��
bool Readn(const int sockfd,char *buffer,const size_t n);

// ���Ѿ�׼���õ�socket��д�����ݡ�
// sockfd���Ѿ�׼���õ�socket���ӡ�
// buffer�����������ݻ������ĵ�ַ��
// n�����������ݵ��ֽ�����
// ����ֵ���ɹ�������n�ֽڵ����ݺ󷵻�true��socket���Ӳ����÷���false��
bool Writen(const int sockfd,const char *buffer,const size_t n);


/************************************************************************************/
// ��������־�ļ�������

// ��־�ļ�������
class CLogFile
{
public:
  FILE   *m_tracefp;           // ��־�ļ�ָ�롣
  char    m_filename[301];     // ��־�ļ�����������þ���·����
  char    m_openmode[11];      // ��־�ļ��Ĵ򿪷�ʽ��һ�����"a+"��
  bool    m_bEnBuffer;         // д����־ʱ���Ƿ����ò���ϵͳ�Ļ�����ƣ�ȱʡ�����á�
  long    m_MaxLogSize;        // �����־�ļ��Ĵ�С����λM��ȱʡ100M��
  bool    m_bBackup;           // �Ƿ��Զ��л�����־�ļ���С����m_MaxLogSize���Զ��л�>
                               // ȱʡ���á�

  // ���캯����
  // MaxLogSize�������־�ļ��Ĵ�С����λM��ȱʡ100M����СΪ10M��
  CLogFile(const long MaxLogSize=100);

  // ����־�ļ���
  // filename����־�ļ�����������þ���·��������ļ����е�Ŀ¼�����ڣ����ȴ���Ŀ¼��
  // openmode����־�ļ��Ĵ򿪷�ʽ����fopen�⺯�����ļ��ķ�ʽ��ͬ��ȱʡֵ��"a+"��
  // bBackup���Ƿ��Զ��л���true-�л���false-���л����ڶ���̵ķ�������У���������\�̹���һ����־�ļ���bBackup����Ϊfalse��
  // bEnBuffer���Ƿ������ļ�������ƣ�true-���ã�false-�����ã�������û���������ôд\����־�ļ��е����ݲ�������д���ļ���ȱʡ�ǲ����á�
  bool Open(const char *filename,const char *openmode=0,bool bBackup=true,\
           bool bEnBuffer=false);

  // �����־�ļ�����m_MaxLogSize��ֵ���Ͱѵ�ǰ����־�ļ�����Ϊ��ʷ��־�ļ������ٴ���\ �µĵ�ǰ��־�ļ���
  // ���ݺ���ļ�������־�ļ������������ʱ�䣬��/tmp/log/filetodb.log.20200101123025��
  // ע�⣬�ڶ���̵ĳ����У���־�ļ������л������ߵĳ����У���־�ļ������л���
  bool BackupLogFile();

  // ������д����־�ļ���fmt�ǿɱ������ʹ�÷�����printf�⺯����ͬ��
  // Write������д�뵱ǰ��ʱ�䣬WriteEx������дʱ�䡣
  bool Write(const char *fmt,...);
  bool WriteEx(const char *fmt,...);

  // �ر���־�ļ�
  void Close();

  ~CLogFile();  // �������������Close������
};


// ���ݾ���·�����ļ�����Ŀ¼���𼶵Ĵ���Ŀ¼��
// pathorfilename������·�����ļ�����Ŀ¼����
// bisfilename��˵��pathorfilename�����ͣ�true-pathorfilename���ļ�����������Ŀ¼����\ȱʡֵΪtrue��
// ����ֵ��true-�����ɹ���false-����ʧ�ܣ��������ʧ�ܣ�ԭ���д�������������1��Ȩ��\���㣻 2��pathorfilename�������ǺϷ����ļ�����Ŀ¼����3�����̿ռ䲻�㡣
bool MKDIR(const char *filename,bool bisfilename=true);


// ���ļ���
// FOPEN��������fopen�⺯�����ļ�������ļ����а�����Ŀ¼�����ڣ��ʹ���Ŀ¼��
// FOPEN�����Ĳ����ͷ���ֵ��fopen������ȫ��ͬ��
// ��Ӧ�ÿ����У���FOPEN��������fopen�⺯����
FILE *FOPEN(const char *filename,const char *mode);


// ��������ʾ��ʱ��ת��Ϊ�ַ�����ʾ��ʱ�䡣
// ltime��������ʾ��ʱ�䡣
// stime���ַ�����ʾ��ʱ�䡣
// fmt������ַ���ʱ��stime�ĸ�ʽ����LocalTime������fmt������ͬ�����fmt�ĸ�ʽ����ȷ��\stime��Ϊ�ա�
void timetostr(const time_t ltime,char *stime,const char *fmt);


/************************************************************************************/
/* 
 ȡ����ϵͳ��ʱ�䣬����������ʾ��ʱ��ת��Ϊ�ַ�����ʾ�ĸ�ʽ��
  stime�����ڴ�Ż�ȡ����ʱ���ַ�����
  timetvl��ʱ���ƫ��������λ���룬0��ȱʡֵ����ʾ��ǰʱ�䣬30��ʾ��ǰʱ��30��֮���ʱ>
��㣬-30��ʾ��ǰʱ��30��֮ǰ��ʱ��㡣
  fmt�����ʱ��ĸ�ʽ��ȱʡ��"yyyy-mm-dd hh24:mi:ss"��Ŀǰ֧�����¸�ʽ��
  "yyyy-mm-dd hh24:mi:ss"���˸�ʽ��ȱʡ��ʽ��
  "yyyymmddhh24miss"
  "yyyy-mm-dd"
  "yyyymmdd"
  "hh24:mi:ss"
  "hh24miss"
  "hh24:mi"
  "hh24mi"
  "hh24"
  "mi"
  ע�⣺
    1��Сʱ�ı�ʾ������hh24������hh����ô����Ŀ����Ϊ�˱��������ݿ��ʱ���ʾ����һ�£�
    2�������г��˳��õ�ʱ���ʽ���������������Ӧ�ÿ������������޸�Դ�������Ӹ����\ ��ʽ֧�֣�
    3�����ú�����ʱ�����fmt��������ʽ��ƥ�䣬stime�����ݽ�Ϊ�ա�
*/
void LocalTime(char *stime,const char *fmt=0,const int timetvl=0);


// ɾ���ַ������ָ�����ַ���
// str����������ַ�����
// chr����Ҫɾ�����ַ���
void DeleteLChar(char *str,const char chr);


// ɾ���ַ����ұ�ָ�����ַ���
// str����������ַ�����
// chr����Ҫɾ�����ַ���
void DeleteRChar(char *str,const char chr);


// ɾ���ַ�����������ָ�����ַ���
// str����������ַ�����
// chr����Ҫɾ�����ַ���
void DeleteLRChar(char *str,const char chr);

// ���ַ����е�Сд��ĸת���ɴ�д�����Բ�����ĸ���ַ���
// str����ת�����ַ�����֧��char[]��string�������͡�
void ToUpper(char *str);
void ToUpper(string &str);

/*************************************************************************************/
/* HTPP��������Ӧ����*/

//��������
void *accept_request(void *arg);
//void accept_request(int client);

// 400 ��Ӧ��BAD REQUEST
void bad_request(int client);

// �򿪾�̬�ļ�����������
void cat(int, FILE *resource);

// 500 ��Ӧ Internal Server Error
void cannot_execute(int client);

// CGI��Ӧpost����
void execute_cgi(int client, const char *path, const char *method,\
                  const char *query_string);

// ��ȡһ�пͻ���������Ϣ
int get_line(int client, char *buf, int size);

// 200 ��Ӧ ������ȷ
void headers(int client, const char *filename);

// 400 ��Ӧ NOT FOUND
void not_found(int client);

// get�����Ӧ
void serve_file(int client, const char *filename);

// 501 ��Ӧ request method not supported
void unimplemented(int client);

 
bool Logfile();


void pthmainexit(void *arg); // �߳���������
