enum MESSAGE_TYPES {
    AUTH_REQUEST,
    AUTH_RESPONSE,
    REGISTER_REQUEST,

    LIST_REQUEST,
    LIST_RESPONSE,

    FILE_REQUEST,
    FILE_RESPONSE,

    FILE_SAVE_REQUEST,
    FILE_SAVE_RESPONSE,
};

/* ======== Авторизация ======== */
struct AUTH_REQUEST {
    enum MESSAGE_TYPES type; // registration or auth type
    char login[255];
    char password[255];
};

enum AUTH_RESPONSE_CODES {
    AUTH_OK,
    AUTH_BADLOGIN, // wrong data for auth or user exists for reg
};

struct AUTH_RESPONSE {
    enum MESSAGE_TYPES type;
    enum AUTH_RESPONSE_CODES status;
};
/* ======== END Авторизация ======== */

/* ======== Запрос списка файла ======== */
struct LIST_REQUEST {
    enum MESSAGE_TYPES type;
};


struct LIST_RESPONSE {
    enum MESSAGE_TYPES type;
    size_t size;
    // char data[unlimited];  sperator \n
};
/* ======== END Запрос списка файла ======== */

/* ======== Получение файла ======== */
enum FILE_REQUEST_ACTIONS {
    FILE_CREATE,
    FILE_EDIT,
    FILE_REMOVE,
};

struct FILE_REQUEST {
    enum MESSAGE_TYPES type;
    enum FILE_REQUEST_ACTIONS action;
    char file_name[255];
};

struct FILE_RESPONSE {
    enum MESSAGE_TYPES type;
    size_t size;

    // char data[unlimited];
};
/* ======== END Получение файла ======== */

/* ======== Отправка файла на сервер ======== */
struct FILE_SAVE_REQUEST {
    enum MESSAGE_TYPES type;
    char file_name[255];
    size_t size;
    // char data[unlimited];  
};

struct FILE_SAVE_RESPONSE {
    enum MESSAGE_TYPES type;
};
/* ======== END Отправка файла на сервер ======== */