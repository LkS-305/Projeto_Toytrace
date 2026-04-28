#include "cli.h"
#include "student_api.h"
#include "trace_runtime.h"

#include <stdio.h>

//esse struct guarda o estado do do trace
struct trace_state {
    int raw_events; //indica se usou --raw_events na chamada do programa
    struct syscall_pairer pairer; //guarda informações para juntar entrada e saída da syscall
};


// Esta funcao e chamada pelo runtime para cada evento de syscall.
//Dependendo do tipo de evento, ela formata de uma forma especifica. Caso o evento não esteja completo, ela o ignora.
//const struct syscall_event *ev yscall guarda syscall, argumentos, retorno, se é entrada ou saída
static void trace_observer(const struct syscall_event *ev, void *userdata)
{
    struct trace_state *state = userdata;
    struct syscall_event completed;
    char line[512];
    int ready;

    //se usou --raw_events entra nesse if e imprime a sair sem formatação
    if (state->raw_events) {
        student_debug_raw_event(ev, line, sizeof(line));
        puts(line);
        return;
    }

    //tenta juntar as informações da entrada e saída da syscall
    ready = student_pair_syscall(&state->pairer, ev, &completed);
    if (ready <= 0) {
        return;
    }

    //quando junta chega nessa linha e formata para imprimir na tela
    student_format_event(&completed, line, sizeof(line));
    puts(line);
}

int main(int argc, char **argv)
{
    struct trace_options opts;
    struct trace_state state = {0};
    int rc;

    rc = parse_args(argc, argv, &opts); //argc --> contador argumentos recebidos, argv --> vetor de strings com os argumentos, opts --> estrutura onde serao armazenadas as opcoes de trace
    if (rc > 0) {
        return 0;
    }
    if (rc < 0) {
        return 2;
    }

    state.raw_events = opts.raw_events;
    rc = trace_program(opts.target_argv, trace_observer, &state);
    return rc < 0 ? 1 : rc;
}
