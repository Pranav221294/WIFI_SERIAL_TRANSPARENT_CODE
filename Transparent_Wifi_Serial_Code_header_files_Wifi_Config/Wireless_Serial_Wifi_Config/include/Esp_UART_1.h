
#define BUF_SIZE_1 (512)
#define EX_UART_NUM UART_NUM_1
#define RD_BUF_SIZE (BUF_SIZE_1)

typedef enum
{
    e_UART_1_data_coming,
    e_UART_1_no_data,
}e_UART_1_reve_data;

void IRAM_ATTR uart_intr_handle(void *arg);
void UART_1_init();
char* get_UART_1_Read_Data();
void setUART1recvstate(e_UART_1_reve_data state);
e_UART_1_reve_data getUART1recvstate(void);
void disable_UART();
void UART_1_deinit();
