Esse arquivo deve explicar:
• onde o programa começa;
O programa começa no arquivo main.c


• onde o processo alvo é criado;
O processo alvo é criado no arquivo trace_runtime.c, 
na função launch_tracee, que devemos fazer na semana 2 a implementação da função alvo.
O main chama trace_program, que esta no trace_runtime, que por sua vez chama o launch_tracee e passa como argumento o argv, 
que é o vetor de argumentos enviado pelo usuario.


• onde o runtime chama o callback;
O callback neste caso é o observer, ou seja, a função trace_observer, que está contida no arquivo main.c.
O runtime chama o callback (trace_observer) sempre que captura um evento de syscall.
É chamado dentro da função trace_program, em 
if (observer != NULL) {
    observer(&ev, userdata);
}
O observer recebe o evento, confere se deve imprimi-lo em modo cru (if --raw-events) ou se deve juntar entrada e saída da syscall. 
Quando o evento está completo, ele é formatado pela função student_format_event e exibe no terminal.


• quais arquivos o grupo deve modificar;
Vamos modificar o trace_runtime, o pairer e o formatter.
O runtime é responsável pelo controle do processo monitorado (criação, execução e captura de syscalls), ou seja tem a lógica principal do tracing, o pairer deve ser implementado para juntar os eventos de entrada e saída de cada syscall, o formatter deve ser implementado para gerar uma saída legível para o usuário a partir dos eventos.


• qual TODO aparece primeiro ao executar o scaffold;
O TODO impresso é: "erro: TODO Semana 2: implementar launch_tracee()\n", encontrado no arquivo trace_runtime

• qual é a principal dúvida técnica do grupo neste momento.
Atualmente, a nossa principal dúvida técnica é em relação a como é feita a coordenação de todas as chamadas de função nos diferentes arquivos, sem que ocorra qualquer tipo de conflito. Além disso, ficamos com certa dificuldade de compreensão devido a todas as diferentes structs utilizadas