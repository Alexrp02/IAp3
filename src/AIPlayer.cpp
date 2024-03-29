#include "AIPlayer.h"
#include "Parchis.h"
#include <algorithm>

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

void AIPlayer::thinkAleatorio(color &c_piece, int &id_piece, int &dice) const
{
    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    // como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    // respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }

    /*
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
            break;
        case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    */
}

bool AIPlayer::move()
{
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const
{
    // El número de mi jugador actual.
    int player = actual->getCurrentPlayerId();
    // Vector que almacenará los dados que se pueden usar para el movimiento.
    vector<int> current_dices_con_especiales;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;
    // Obtengo el vector de dados que puedo usar para el movimiento.
    // En este caso elijo todos, tanto normales como especiales.
    // Importante: puedo acceder a solo dados normales o especiales por separado,
    // o a todos a la vez:
    // - actual->getAvailableNormalDices(player) -> solo dados normales
    // - actual->getAvailableSpecialDices(player) -> solo dados especiales
    // - actual->getAllAvailableDices(player) -> todos los dados
    // Importante 2: los "available" me dan los dados que puedo usar en el turno actual.
    // Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
    // Puedo saber qué más dados tengo, aunque no los pueda usar en este turno, con:
    // - actual->getNormalDices(player) -> todos los dados normales
    // - actual->getSpecialDices(player) -> todos los dados especiales
    // - actual->getAllDices(player) -> todos los dados
    current_dices_con_especiales = actual->getAllAvailableDices(player);
    // En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;
    for (int i = 0; i < current_dices_con_especiales.size(); i++)
    {
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, current_dices_con_especiales[i]);
        // Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
        if (current_pieces.size() > 0)
        {
            current_dices_que_pueden_mover_ficha.push_back(current_dices_con_especiales[i]);
        }
    }
    // Si no tengo ninún dado que pueda mover fichas, paso turno con un dado al azar (la macro SKIP_TURN me permite no mover).
    if (current_dices_que_pueden_mover_ficha.size() == 0)
    {
        dice = current_dices_con_especiales[rand() % current_dices_con_especiales.size()];
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
    // En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.
    else
    {
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, dice);
        // Muevo una ficha al azar de entre las que se pueden mover.
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);
    }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const
{
    // Elijo el dado haciendo lo mismo que el jugador anterior.
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    // Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
    // Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
    // el dado especial a) la que esté más adelantada
    // (equivalentemente, la más cercana a la meta).
    int player = actual->getCurrentPlayerId();
    vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);
    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;
    for (int i = 0; i < current_pieces.size(); i++)
    {
        // distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
        if (distancia_meta < min_distancia_meta)
        {
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
    }
    // Si no he encontrado ninguna ficha, paso turno.
    if (id_ficha_mas_adelantada == -1)
    {
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
    // En caso contrario, moveré la ficha más adelantada.
    else
    {
        id_piece = id_ficha_mas_adelantada;
        c_piece = col_ficha_mas_adelantada;
    }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
    // Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual.
    // Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren().
    // Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable
    // sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama.
    ParchisBros hijos = actual->getChildren();
    bool me_quedo_con_esta_accion = false;
    // La clase ParchisBros viene con un iterador muy útil y sencillo de usar.
    // Al hacer begin() accedemos al primer hijo de la rama,
    // y cada vez que hagamos ++it saltaremos al siguiente hijo.
    // Comparando con el iterador end() podemos consultar cuándo hemos terminado de visitar los hijos.
    for (ParchisBros::Iterator it = hijos.begin();
         it != hijos.end() and !me_quedo_con_esta_accion; ++it)
    {

        Parchis siguiente_hijo = *it;                                                   // Accedemos al tablero hijo con el operador de indirección.
        if (siguiente_hijo.isEatingMove() or                                            // Si he comido ficha o …
            siguiente_hijo.isGoalMove() or                                              // … una ficha ha llegado a la meta o …
            (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador) // … he ganado
        )
        {

            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();    // Guardo color de la ficha movida.
            id_piece = it.getMovedPieceId(); // Guardo id de la ficha movida.
            dice = it.getMovedDiceValue();   // Guardo número de dado movido.
        }
    }
    // Si he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia.
    // (Ya lo he hecho antes, cuando les he asignado los valores con el iterador).
    // Si no, muevo la ficha más adelantada como antes.
    if (!me_quedo_con_esta_accion)
    {
        thinkFichaMasAdelantada(c_piece, id_piece, dice);
    }
}

int piezasEnPasillo(const Parchis &estado, int jugador)
{
    // Devuelve el número de piezas que tiene el jugador en el pasillo
    int piezas = 0;
    vector<color> colores = estado.getPlayerColors(jugador);
    for (auto color : colores)
    {
        for (int i = 0; i < 3; i++)
        {
            if (estado.distanceToGoal(color, i) <= 8)
                piezas++;
        }
    }
    return piezas;
}

double AIPlayer::valoracion3(const Parchis &estado, int jugador)
{
    // Restamos la distancia que hay desde todas las fichas del jugador hasta la meta.
    double valoracion = 0;

    // Si se llega a la meta y es el jugador, devuelve victoria
    if (estado.gameOver() && estado.getWinner() == jugador)
    {
        return gana;
    }
    else if (estado.gameOver())
    {
        return pierde;
    }

    vector<color> colores = estado.getPlayerColors(jugador);
    for (int i = 0; i < colores.size(); i++)
    {
        // Por cada ficha en casa, restamos 5
        valoracion -= estado.piecesAtHome(colores[i]) * 5;
        // Por cada ficha en la casilla final sumamos 74
        valoracion += estado.piecesAtGoal(colores[i]) * 75;
        color c = colores[i];

        // // Si tenemos fichas de este color en la meta y en el último movimiento se ha movido una ficha de este color, sumamos 10 por cada ficha
        // if (estado.piecesAtGoal(colores[i]) > 0 && get<0>(estado.getLastAction()) == colores[i])
        // {
        //     valoracion += estado.piecesAtGoal(colores[i]) * 10;
        // }

        for (int j = 0; j < 3; j++)
        {
            // Si tenemos dos fichas de este color en la meta, sumamos un tercio de la distancia de la que queda
            if (estado.piecesAtGoal(colores[i]) > 0)
            {
                valoracion += estado.distanceToGoal(c, j) / 3;
            }
            valoracion -= estado.distanceToGoal(c, j);
        }
    }

    // Le sumamos la distancia que tiene el otro jugador
    int oponente = (jugador + 1) % 2;
    int valoracion_oponente = 0;
    colores = estado.getPlayerColors(oponente);
    for (int i = 0; i < colores.size(); i++)
    {
        // Por cada ficha en casa, sumamos 5
        valoracion_oponente += estado.piecesAtHome(colores[i]) * 5;
        // Por cada ficha en la casilla final restamos 74
        valoracion_oponente -= estado.piecesAtGoal(colores[i]) * 75;
        color c = colores[i];

        // // Si el rival tiene fichas de este color en la meta y en el último movimiento se ha movido una ficha de este color, restamos 10 por cada ficha
        // if (estado.piecesAtGoal(colores[i]) > 0 && get<0>(estado.getLastAction()) == colores[i])
        // {
        //     valoracion -= estado.piecesAtGoal(colores[i]) * 10;
        // }

        for (int j = 0; j < 3; j++)
        {
            // Si tenemos dos fichas de este color en la meta, restamos un tercio de la distancia de la que queda
            if (estado.piecesAtGoal(colores[i]) > 0)
            {
                valoracion -= estado.distanceToGoal(c, j) / 3;
            }
            valoracion_oponente += estado.distanceToGoal(c, j);
        }
    }

    // Si tenemos el caparazón azul y no lo cogimos en el anterior turno y el rival tiene alguna ficha a menos de 8 casillas de la meta, restaremos 75
    if (piezasEnPasillo(estado, oponente) > 0 and find(estado.getSpecialDices(jugador).begin(), estado.getSpecialDices(jugador).end(), blue_shell) != estado.getSpecialDices(jugador).end())
    {
        valoracion -= 75;
    }

    // Si tenemos más de una pieza en el pasillo final y el rival no tiene el caparazón azul, sumamos 67 por cada pieza
    if (piezasEnPasillo(estado, jugador) > 0 and find(estado.getSpecialDices(oponente).begin(), estado.getSpecialDices(oponente).end(), blue_shell) == estado.getSpecialDices(oponente).end())
    {
        valoracion += 67 * piezasEnPasillo(estado, jugador);
    }

    // Si en el anterior turno cogimos la bala, el caparazon azul o la estrella sumamos 40
    if ((estado.getItemAcquired() == bullet or estado.getItemAcquired() == blue_shell or estado.getItemAcquired() == star) and estado.getCurrentPlayerId() != jugador)
    {
        valoracion += 40;
    }

    // Si en el anterior turno el rival cogió la bala, el caparazon azul o la estrella restamos 40
    if ((estado.getItemAcquired() == bullet or estado.getItemAcquired() == blue_shell or estado.getItemAcquired() == star) and estado.getCurrentPlayerId() == jugador)
    {
        valoracion -= 40;
    }

    // Si en el último turno obtuvimos el champiñon, sumamos 8
    if (estado.getItemAcquired() == mushroom and estado.getCurrentPlayerId() != jugador)
    {
        valoracion += 8;
    }
    else if (estado.getItemAcquired() == mushroom and estado.getCurrentPlayerId() == jugador)
    {
        valoracion -= 8;
    }

    // Si en el turno anterior el jugador se comió una ficha, sumamos 20
    if (estado.isEatingMove() and estado.getCurrentPlayerId() == jugador)
    {
        valoracion += 20;
    }
    else if (estado.isEatingMove() and estado.getCurrentPlayerId() != jugador)
    {
        valoracion -= 20;
    }

    // Si en el turno anterior se metió una ficha en la meta, sumamos 10 o restamos 10 dependiendo de si fue el jugador o el rival
    if (estado.isGoalMove() and estado.getCurrentPlayerId() == jugador)
    {
        valoracion += 10;
    }
    else if (estado.isGoalMove() and estado.getCurrentPlayerId() != jugador)
    {
        valoracion -= 10;
    }

    // Si el jugador tiene la bala, sumamos 40
    if (find(estado.getSpecialDices(jugador).begin(), estado.getSpecialDices(jugador).end(), bullet) != estado.getSpecialDices(jugador).end())
    {
        valoracion += 40;
    }
    if (find(estado.getSpecialDices(oponente).begin(), estado.getSpecialDices(oponente).end(), bullet) != estado.getSpecialDices(oponente).end())
    {
        valoracion -= 40;
    }

    // Si el jugador tiene el champiñon, sumamos 8
    if (find(estado.getSpecialDices(jugador).begin(), estado.getSpecialDices(jugador).end(), mushroom) != estado.getSpecialDices(jugador).end())
    {
        valoracion += 8;
    }
    if (find(estado.getSpecialDices(oponente).begin(), estado.getSpecialDices(oponente).end(), mushroom) != estado.getSpecialDices(oponente).end())
    {
        valoracion -= 8;
    }

    // Si se ha usado un dado especial y se ha llegado a la meta con alguna ficha, sumamos 50
    if (estado.isGoalMove() and estado.isSpecialDice(estado.getLastDice()) and estado.getCurrentPlayerId() == jugador)
    {
        valoracion += 10;
    }
    else if (estado.isGoalMove() and estado.isSpecialDice(estado.getLastDice()) and estado.getCurrentPlayerId() != jugador)
    {
        valoracion -= 10;
    }

    return valoracion + valoracion_oponente;
}

double AIPlayer::valoracion2(const Parchis &estado, int jugador)
{
    // Restamos la distancia que hay desde todas las fichas del jugador hasta la meta.
    double valoracion = 0;

    // Si se llega a la meta y es el jugador, devuelve victoria
    if (estado.gameOver() && estado.getWinner() == jugador)
    {
        return gana;
    }
    else if (estado.gameOver())
    {
        return pierde;
    }

    vector<color> colores = estado.getPlayerColors(jugador);
    for (int i = 0; i < colores.size(); i++)
    {
        // Si tengo más fichas de este color que del otro, sumamos 15
        if (estado.piecesAtGoal(colores[i]) > estado.piecesAtGoal(colores[(i + 1) % 2]))
        {
            valoracion += 15;
        }
        // Por cada ficha en casa, restamos 10
        valoracion -= estado.piecesAtHome(colores[i]) * 10;
        // Por cada ficha en la meta, sumamos 10
        valoracion += estado.piecesAtGoal(colores[i]) * 10;
        color c = colores[i];

        for (int j = 0; j < 3; j++)
        {
            valoracion += 73 - estado.distanceToGoal(c, j);
            // Si la ficha está a salvo, sumaremos 5 (que es lo que nos cuesta sacarla de casa)
            if (estado.isSafePiece(c, j))
            {
                valoracion += 5;
            }
        }
    }

    // Dependiendo de los dados especiales que tenga el jugador, sumamos una puntuación distinta
    auto dadosEspeciales = estado.getSpecialDices(jugador);
    for (auto dado : dadosEspeciales)
    {
        switch (dado)
        {
        case star:
            valoracion += 50;
            break;
        case bullet:
            valoracion += 40;
            break;
        case blue_shell:
            valoracion += 40;
            break;
        case mushroom:
            valoracion += 8;
            break;
        case red_shell:
            valoracion += 30;
            break;
        case shock:
            valoracion += 27.5;
            break;
        case horn:
            valoracion += 25;
            break;
        default:
            break;
        }
    }

    // Si el jugador se ha comido una ficha en el último turno, sumamos 25 (20 de movimiento más los 5 que gasta el rival para sacarla de casa)
    if (estado.isEatingMove() and estado.getCurrentPlayerId() == jugador)
    {
        valoracion += 25;
    }

    // Le sumamos la distancia que tiene el otro jugador
    int oponente = (jugador + 1) % 2;
    int valoracion_oponente = 0;
    colores = estado.getPlayerColors(oponente);
    for (int i = 0; i < colores.size(); i++)
    {
        // Si tengo más fichas de este color que del otro, sumamos 15
        if (estado.piecesAtGoal(colores[i]) > estado.piecesAtGoal(colores[(i + 1) % 2]))
        {
            valoracion_oponente += 15;
        }
        // Por cada ficha en casa, restamos 10
        valoracion_oponente -= estado.piecesAtHome(colores[i]) * 10;
        // Por cada ficha en la meta, sumamos 10
        valoracion_oponente += estado.piecesAtGoal(colores[i]) * 10;
        color c = colores[i];

        for (int j = 0; j < 3; j++)
        {
            valoracion_oponente += 73 - estado.distanceToGoal(c, j);
            // Si la ficha está a salvo, sumaremos 5 (que es lo que nos cuesta sacarla de casa)
            if (estado.isSafePiece(c, j))
            {
                valoracion_oponente += 5;
            }
        }
    }
    // Dependiendo de los dados especiales que tenga el rival, sumamos una puntuación distinta
    dadosEspeciales = estado.getSpecialDices(oponente);
    for (auto dado : dadosEspeciales)
    {
        switch (dado)
        {
        case star:
            valoracion_oponente += 50;
            break;
        case bullet:
            valoracion_oponente += 40;
            break;
        case blue_shell:
            valoracion_oponente += 40;
            break;
        case mushroom:
            valoracion_oponente += 8;
            break;
        case red_shell:
            valoracion_oponente += 30;
            break;
        case shock:
            valoracion_oponente += 27.5;
            break;
        case horn:
            valoracion_oponente += 25;
            break;
        default:
            break;
        }
    }

    // Si el oponente se ha comido una ficha en el último turno, sumamos 25 (20 de movimiento más los 5 que gasta el jugador para sacarla de casa)
    if (estado.isEatingMove() and estado.getCurrentPlayerId() == oponente)
    {
        valoracion_oponente += 25;
    }

    return valoracion - valoracion_oponente;
}

double AIPlayer::valoracionDistancia(const Parchis &estado, int jugador)
{
    // Sumamos la distancia que hay desde todas las fichas del jugador hasta la meta.
    double valoracion = 0;

    // Si se llega a la meta y es el jugador, devuelve victoria
    if (estado.gameOver() && estado.getWinner() == jugador)
    {
        return gana;
    }
    else if (estado.gameOver())
    {
        return pierde;
    }

    // Si se ha usado un dado especial y se ha llegado a la meta con alguna ficha, devolvemos sumamos 50
    if (estado.isGoalMove() and estado.isSpecialDice(estado.getLastDice()) and estado.getCurrentPlayerId() != jugador)
    {
        valoracion += 60;
    }

    // Si tiene fichas en casa, resta 10 puntos por cada una.
    for (auto &color : estado.getPlayerColors(jugador))
    {
        if (estado.piecesAtHome(color) > 0)
        {
            valoracion -= 10 * estado.piecesAtHome(color);
        }
    }

    // Por cada ficha del rival en casa, suma 10 puntos
    for (auto &color : estado.getPlayerColors((jugador + 1) % 2))
    {
        if (estado.piecesAtHome(color) > 0)
        {
            valoracion += 10 * estado.piecesAtHome(color);
        }
    }

    // Si es un movimiento de comer y se come una ficha enemiga, suma 20 puntos.
    if (jugador == 0)
    {
        if (estado.isEatingMove() && (get<0>(estado.eatenPiece()) == blue or get<0>(estado.eatenPiece()) == green))
        {
            valoracion += 20;
        }
        else
        {
            valoracion -= 20;
        }
    }
    else
    {
        if (estado.isEatingMove() && (get<0>(estado.eatenPiece()) == red or get<0>(estado.eatenPiece()) == yellow))
        {
            valoracion += 20;
        }
        else
        {
            valoracion -= 20;
        }
    }

    vector<color> colores = estado.getPlayerColors(jugador);
    for (int i = 0; i < colores.size(); i++)
    {
        color c = colores[i];
        for (int j = 0; j < 3; j++)
        {
            valoracion -= estado.distanceToGoal(c, j);
        }
    }
    // Le sumamos la distancia que tiene el otro jugador
    int oponente = (jugador + 1) % 2;
    int valoracion_oponente = 0;
    colores = estado.getPlayerColors(oponente);
    for (int i = 0; i < colores.size(); i++)
    {
        color c = colores[i];
        for (int j = 0; j < 3; j++)
        {
            valoracion_oponente += estado.distanceToGoal(c, j);
        }
    }
    if (valoracion_oponente < 40 and find(estado.getAvailableSpecialDices(jugador).begin(), estado.getAvailableSpecialDices(jugador).end(), blue_shell) != estado.getAvailableSpecialDices(jugador).end())
    {
        valoracion += 50;
    }
    return valoracion + valoracion_oponente;
}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.

    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

double AIPlayer::Poda_AlfaBeta(const Parchis &parchis, int jugador, int profundidad, int profundidad_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const
{
    // cout << "Profundidad: " << profundidad << endl;
    // Si hemos llegado a la profundidad máxima, devolvemos la valoración del estado actual.
    if (profundidad == profundidad_max or parchis.gameOver())
    {
        return heuristic(parchis, jugador);
    }
    // Creamos las variables alpha y beta que se propagan desde el padre.
    double alfaActual = alpha;
    double betaActual = beta;

    // Si no, generamos los hijos del estado actual.
    ParchisBros hijos = parchis.getChildren();
    // Si hay hijos, iteramos sobre ellos llamando a la poda Alfa Beta, modificando alfa o beta dependiendo de si es un jugador u otro.
    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end(); ++it)
    {
        // Si el jugador es el jugador actual, actualizaremos alfa que es el mayor valor que puede tomar este nodo
        if (jugador == parchis.getCurrentPlayerId())
        {
            double valor = Poda_AlfaBeta(*it, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice, alfaActual, betaActual, heuristic);

            // Si la profundidad es 1, es decir, la siguiente acción que vamos a realizar y el alfa que tenemos es mayor que el que teníamos, actualizamos la acción que vamos a realizar.
            if (profundidad == 0 && valor > alfaActual)
            {
                c_piece = get<0>((*it).getLastAction());
                id_piece = get<1>((*it).getLastAction());
                dice = get<2>((*it).getLastAction());
            }

            alfaActual = max(alfaActual, valor);

            // Si alfa es mayor o igual que beta, devolvemos alfa, es decir, podamos los siguientes hijos.
            if (alfaActual >= betaActual)
            {
                return alfaActual;
            }
        }
        else
        {
            double valor = Poda_AlfaBeta(*it, jugador, profundidad + 1, profundidad_max, c_piece, id_piece, dice, alfaActual, betaActual, heuristic);
            // Si el jugador es el oponente, actualizaremos beta que es el menor valor que puede tomar este nodo
            // Si la profundidad es 1, es decir, la siguiente acción que vamos a realizar y el beta que tenemos es mayor que el alpha que teníamos, actualizamos la acción que vamos a realizar.
            if (profundidad == 0 && betaActual > alpha)
            {
                c_piece = get<0>((*it).getLastAction());
                id_piece = get<1>((*it).getLastAction());
                dice = get<2>((*it).getLastAction());
            }
            betaActual = min(betaActual, valor);

            // Si alfa es mayor o igual que beta, devolvemos beta, es decir, podamos los siguientes hijos.
            if (alfaActual >= betaActual)
            {
                return betaActual;
            }
        }
    }
    if (jugador == parchis.getCurrentPlayerId())
        return alfaActual;
    else
        return betaActual;
}

void AIPlayer::think(color &c_piece, int &id_piece, int &dice) const
{
    // switch (id)
    // {
    // case 0:
    //     thinkAleatorio(c_piece, id_piece, dice);
    //     break;
    // case 1:
    //     thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    //     break;
    // case 2:
    //     thinkFichaMasAdelantada(c_piece, id_piece, dice);
    //     break;
    // case 3:
    //     thinkMejorOpcion(c_piece, id_piece, dice);
    //     break;
    // }

    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor;                           // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    // valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    // cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch (id)
    {
    case 0:
        valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
        break;
    case 1:
        valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, valoracion2);
        break;
    case 2:
        valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, valoracionDistancia);
        break;
    case 3:
        thinkAleatorioMasInteligente(c_piece, id_piece, dice);
        break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}