int viaggio(int itinerario[20], int logFd, int *fdMaPrecedente, int i);
void trenoRegistro(int id, int mappa, int itinerario[20]);
int inizializzazioneLogFile(int id, int itinerario[20]);
void rilascioUltimoBinario(int logFd, int fdMaPrecedente, int itinerario[20], int i);