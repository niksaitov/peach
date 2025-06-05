window.onload = setBoardFromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

function setBoardFromUserFEN() {

    const fenStr = document.getElementById('fenInput').value;

    if(fenStr === ''){
        return
    }else{

        const pieces = document.querySelectorAll('.piece');
        pieces.forEach(piece => piece.remove());
        setBoardFromFEN(fenStr)

    }
    
}

function setBoardFromFEN(fenStr){

    const fenParts = fenStr.split(' ');
    const piecePlacement = fenParts[0];
    let rank = 0, file = 0;

    for (let i = 0; i < piecePlacement.length; i++){
        
        const char = piecePlacement.charAt(i);
        
        if (char === '/'){

            rank++;
            file = 0;

        }else if (isNaN(char)){

            const squareIndex = rank * 8 + file;
            const piece = getPieceCode(char);
            const color = getPieceColor(char);
            const type = char;
            
            setPiece(squareIndex, piece, color, type);
            file++;

        }else{
            file += parseInt(char);
        }
        
    }

}

function getPieceCode(fenChar){
    switch (fenChar) {

        case 'p': return '&#9823;';
        case 'n': return '&#9822;';
        case 'b': return '&#9821;';
        case 'r': return '&#9820;';
        case 'q': return '&#9819;';
        case 'k': return '&#9818;';
        case 'P': return '&#9817;';
        case 'N': return '&#9816;';
        case 'B': return '&#9815;';
        case 'R': return '&#9814;';
        case 'Q': return '&#9813;';
        case 'K': return '&#9812;';
        default: return '';
        
    }

}

function getPieceColor(fenChar){

    if(fenChar === fenChar.toUpperCase()){
        return "white"
    }else{
        return "black"
    }

}

function setPiece(squareIndex, piece, color, type){

    const squareElement = document.getElementById(squareIndex);
    squareElement.innerHTML = `<div class="piece" draggable="true" data-color="${color}" data-type="${type}">${piece}</div>`;

}

function getFENfromBoard(){

    let fenStr = '';
    let emptySquareCount = 0;

    for(let rank = 0; rank < 8; rank++){

        for(let file = 0; file < 8; file++){

            const squareIndex = rank * 8 + file;
            const piece = getPieceFromSquare(squareIndex);

            if(piece === ''){
                emptySquareCount++;
            }else{

                if (emptySquareCount > 0) {

                    fenStr += emptySquareCount;
                    emptySquareCount = 0;

                }

                fenStr += piece;

            }

        }

        if (emptySquareCount > 0) {

            fenStr += emptySquareCount;
            emptySquareCount = 0;

        }

        if(rank !== 7){
            fenStr += '/';
        }

    }

    outputElement = document.getElementById('fenOutput');
    outputElement.value = fenStr;
    
}

function getPieceFromSquare(squareIndex){

    const squareElement = document.getElementById(squareIndex);
    const pieceElement = squareElement.querySelector('.piece');

    if(pieceElement !== null){
        return pieceElement.getAttribute("data-type");
    }else{
        return '';
    }
    
}