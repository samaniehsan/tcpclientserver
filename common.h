
#ifndef BYTE
typedef char BYTE;
#endif

class SocketHelper {
public:
    static int printError(const char * errTitle);
    static bool addStudent(
        int socket,
        unsigned int studentId,
        const char * firstName,
        const char * lastName,
        BYTE score);
};

enum Action_Code {
  Action_Code_Display_All = 1,
  Action_Code_Display_Score = 2,
  Action_Code_Display_Id = 3,
  Action_Code_Display_Add = 4,
  Action_Code_Display_Delete = 5
};

typedef struct {
    BYTE code;
} RequestBody;

typedef struct {
  unsigned int studentId;
} StudentRequest;

typedef struct {
  BYTE score;
} ScoreRequest;

typedef struct {
  unsigned int studentId;
  char firstName[11];
  char lastName[11];
  BYTE score;
} Student;


