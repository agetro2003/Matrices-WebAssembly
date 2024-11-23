// Esperar a que el módulo de WebAssembly esté listo
Module.onRuntimeInitialized = () => {
    const rows = document.getElementById('rows');
    const cols = document.getElementById('cols');
    const resultDisplay = document.getElementById('result');
    const button = document.getElementById('start');
    const threadsInput = document.getElementById('num_threads');
    // Evento para realizar la multiplicación de matrices
    button.addEventListener('click', () => {
        const numRows = parseInt(rows.value);
        const numCols = parseInt(cols.value);
        const threads = parseInt(threadsInput.value);
        // Llamar a la función de multiplicación de matrices
        try {
        const start_time = performance.now();
        console.log(start_time);
        const  result = Module.ccall(
            'calculate_matrices', // Nombre de la función
            'number', // Tipo de retorno
            ['number', 'number', 'number'], // Tipos de argumentos
            [numRows, numCols, threads] // Argumentos
        );
        console.log(result);
        const end_time = performance.now();
        console.log(end_time);
        let final_time = end_time - start_time;
        resultDisplay.innerHTML = `El tiempo de ejecucion fue de: ${final_time} ms`;
        }
        catch (error) {
            console.error(error);
            resultDisplay.innerHTML = 'Ocurrió un error al multiplicar las matrices';
        }
    });
};    