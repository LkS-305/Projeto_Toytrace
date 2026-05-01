#include "trace_runtime.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#if !defined(__x86_64__)
#error "Este runtime didatico suporta apenas Linux x86_64."
#endif

//transforma os registradores da CPU em um evento do toytrace, os registradores mostram qual syscall foi chamada, quais argumentos foram passados, qual foi o retorno
static void fill_event_from_regs(pid_t pid,
                                 int entering, //identifica se ta na entrada ou saida da syscall
                                 const struct user_regs_struct *regs, //onde estarão os registradores lidos do processo filho
                                 struct syscall_event *ev) //struct em q guardaremos os dados organizados
{
    /*
     * TODO Semana 4:
     *
     * Preencha struct syscall_event usando os registradores x86_64.
     *
     * Dicas:
     * - regs->orig_rax contem o numero da syscall.
     * - regs->rax contem o retorno, valido na saida.
     * - os seis argumentos ficam em rdi, rsi, rdx, r10, r8 e r9.
     * - ev->entering deve copiar o parametro entering.
     */
    memset(ev, 0, sizeof(*ev));
    //guardando qual processo fez a syscall e se é entrada ou saída
    ev->pid = pid;
    ev->entering = entering;
}

//aqui criamos o processo que será monitorado
//char *const argv[] - programa alvo e seus argumentos
static pid_t launch_tracee(char *const argv[])
{
    /*
     * TODO Semana 2:
     *
     * Crie o processo monitorado.
     *
     * Fluxo esperado:
     * - fork()
     * - no filho:
     *   - ptrace(PTRACE_TRACEME, ...)
     *   - raise(SIGSTOP)
     *   - execvp(argv[0], argv)
     * - no pai:
     *   - retornar o pid do filho
     *
     * Em erro, imprima uma mensagem com perror() e retorne -1.
     */
    fprintf(stderr, "erro: TODO Semana 2: implementar launch_tracee()\n");
    return -1;
}

//pai espera o filho parar pela primeira vez quando o filho fizer SIGSTOP para ent o pai configurar o ptrace antes do filho executar o programa alvo
static int wait_for_initial_stop(pid_t child)
{
    /*
     * TODO Semana 2:
     *
     * O filho chama raise(SIGSTOP) antes de executar o programa alvo.
     * O pai precisa esperar essa parada inicial com waitpid().
     *
     * Retorne 0 se o filho parou como esperado, -1 em erro.
     */
    fprintf(stderr, "erro: TODO Semana 2: implementar wait_for_initial_stop()\n");
    return -1;
}

//onde configurar opções do ptrace
static int configure_trace_options(pid_t child)
{
    /*
     * TODO Semana 3:
     *
     * Configure PTRACE_O_TRACESYSGOOD com PTRACE_SETOPTIONS.
     * Isso ajuda a diferenciar paradas de syscall de outros sinais.
     */
    fprintf(stderr, "erro: TODO Semana 3: implementar configure_trace_options()\n");
    return -1;
}

//mandar o filho continuar rodando até chegar na próxima syscall.
static int resume_until_next_syscall(pid_t child, int signal_to_deliver)
{
    /*
     * TODO Semana 3:
     *
     * Use ptrace(PTRACE_SYSCALL, ...) para deixar o filho executar ate a
     * proxima entrada ou saida de syscall.
     *
     * signal_to_deliver deve ser repassado como quarto argumento do ptrace.
     */
    fprintf(stderr, "erro: TODO Semana 3: implementar resume_until_next_syscall()\n");
    return -1;
}

//espera o filho parar e identifica o motivo da parada (syscall, término ou erro)
static int wait_for_syscall_stop(pid_t child, int *status)
{
    /*
     * TODO Semana 3:
     *
     * Espere o filho com waitpid().
     *
     * Retorne:
     *   1 se a parada foi uma parada de syscall;
     *   0 se o filho terminou normalmente ou por sinal;
     *  -1 em erro.
     *
     * Dicas:
     * - WIFEXITED e WIFSIGNALED indicam fim do processo.
     * - WIFSTOPPED indica que o processo parou.
     * - com PTRACE_O_TRACESYSGOOD, syscall-stops aparecem com bit 0x80.
     * - paradas SIGTRAP comuns nao devem ser entregues de volta ao filho.
     */
    fprintf(stderr, "erro: TODO Semana 3: implementar wait_for_syscall_stop()\n");
    return -1;
}

//função principal
int trace_program(char *const argv[],
                  trace_observer_fn observer,
                  void *userdata)
{
    pid_t child;
    int status = 0;
    int entering = 1;

    //confere se tem programa alvo
    if (argv == NULL || argv[0] == NULL) {
        fprintf(stderr, "erro: programa alvo ausente\n");
        return -1;
    }

    //cria o processo
    child = launch_tracee(argv);
    if (child < 0) {
        return -1;
    }

    //espera o filho parar
    if (wait_for_initial_stop(child) < 0) {
        return -1;
    }

    if (configure_trace_options(child) < 0) {
        return -1;
    }

    if (resume_until_next_syscall(child, 0) < 0) {
        return -1;
    }

    while (1) {
        struct user_regs_struct regs;
        struct syscall_event ev;
        int stop_kind;

        //confere se o processo terminou ou parou em uma syscall
        stop_kind = wait_for_syscall_stop(child, &status);
        if (stop_kind < 0) {
            return -1;
        }
        //se o filho terminou normalmente, retorna o código de saída dele
        if (stop_kind == 0) {
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            //se filho morreu por sinal, retorna um código baseado nisso
            if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return 0;
        }

        /*
         * TODO Semana 4:
         *
         * Use PTRACE_GETREGS para preencher regs.
         * Depois chame fill_event_from_regs() e observer().
         */
        memset(&regs, 0, sizeof(regs));
        // monta um evento de syscall com os registradores guardados
        fill_event_from_regs(child, entering, &regs, &ev);

        //chama o callback para formatar e imprimir (mas so imprime se ja tem entrada e saida registrados)
        if (observer != NULL) {
            observer(&ev, userdata);
        }

        //alterna entre entrada e saída da syscall. quando é entrada o entering fica =1 e saida fica =0
        entering = !entering;

        //manda continuar até a próxima syscall
        if (resume_until_next_syscall(child, 0) < 0) {
            return -1;
        }
    }
}
