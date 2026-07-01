#include <stdio.h>      // printf(), perror()
#include <stdlib.h>     // exit()
#include <fcntl.h>      // open()
#include <unistd.h>     // read(), write(), close()
#include <termios.h>    // configuración UART Linux
#include <signal.h>     // Ctrl+C (SIGINT)
#include <poll.h>       // poll()

#define SERIAL_PORT "/dev/ttyUSB0"

int fd;                         // descriptor del puerto serie
struct termios oldtty;         // configuración original

void cerrar_programa(int sig)
{
    tcsetattr(fd, TCSANOW, &oldtty);  // restaurar configuración
    close(fd);                        // cerrar puerto
    printf("\nPuerto cerrado.\n");
    exit(0);
}

int main(void)
{
    struct termios tty;   // nueva configuración UART

    signal(SIGINT, cerrar_programa); // Ctrl+C -> cerrar_programa()

    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY); // abrir UART

    if (fd < 0)
    {
        perror("No se pudo abrir el puerto serie");
        return 1;
    }

    tcgetattr(fd, &oldtty); // leer config actual
    tty = oldtty;           // copiar configuración base

    cfmakeraw(&tty);        // modo raw, sin eco ni procesamiento

    cfsetispeed(&tty, B9600); // RX a 9600 baudios
    cfsetospeed(&tty, B9600); // TX a 9600 baudios

    tty.c_cflag |= CLOCAL | CREAD; // habilitar recepción
    tty.c_cflag &= ~PARENB;        // sin paridad
    tty.c_cflag &= ~CSTOPB;        // 1 bit stop
    tty.c_cflag &= ~CSIZE;         // limpiar tamaño previo
    tty.c_cflag |= CS8;            // 8 bits de datos

    tty.c_lflag &= ~ECHO;          // desactivar eco

    tty.c_cc[VMIN]  = 1;           // esperar 1 byte
    tty.c_cc[VTIME] = 0;           // sin timeout

    tcflush(fd, TCIFLUSH);         // limpiar buffer RX
    tcsetattr(fd, TCSANOW, &tty);  // aplicar configuración

    printf("Conectado a %s a 9600 8N1\n", SERIAL_PORT);
    printf("Escriba texto y presione ENTER para enviar. Ctrl+C para salir.\n\n");

    // Monitoreo concurrente de STDIN y el puerto serie con poll()
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;  // Entrada de teclado
    fds[0].events = POLLIN;
    fds[1].fd = fd;            // Puerto serie
    fds[1].events = POLLIN;

    while (1)
    {
        if (poll(fds, 2, -1) > 0) // Esperar de forma indefinida por eventos
        {
            // Evento en la entrada de teclado (STDIN)
            if (fds[0].revents & POLLIN)
            {
                char c;
                if (read(STDIN_FILENO, &c, 1) == 1)
                {
                    write(fd, &c, 1); // Mandar byte a la BluePill
                }
            }
            // Evento en el puerto serie (BluePill)
            if (fds[1].revents & POLLIN)
            {
                char c;
                if (read(fd, &c, 1) == 1)
                {
                    putchar(c);       // Imprimir byte recibido
                    fflush(stdout);   // Mostrar inmediatamente
                }
            }
        }
    }

    return 0;
}
