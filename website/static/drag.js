let draggedPiece = null;
let draggedPieceStartIndex = null;
let halfMove = 0;
let pgnStr = "";

const pieces = document.querySelectorAll('.piece');

pieces.forEach(piece =>{

    piece.addEventListener('dragstart', handleDragStart);
    piece.addEventListener('dragend', handleDragEnd);
    piece.addEventListener('drop', handleDropPiece)

});

const squares = document.querySelectorAll('.square');

squares.forEach(square =>{

    square.addEventListener('dragover', handleDragOver);
    square.addEventListener('drop', handleDropSquare);

});

function handleDragStart(event){

    const piece = event.target;
    draggedPiece = piece;
    draggedPieceStartIndex = parseInt(piece.parentNode.id);

}

function handleDragOver(event){
    event.preventDefault();
}

function handleDropSquare(event){

    event.preventDefault();
    const dropSquareElement = event.target;

    if (!dropSquareElement.querySelector(".piece")) {

        const dropSquareIndex = parseInt(dropSquareElement.id);
        movePiece(draggedPieceStartIndex, dropSquareIndex);
        recordMove(draggedPieceStartIndex, dropSquareIndex);

    }

}

function handleDropPiece(event){

    event.preventDefault();
    const dropSquarePiece = event.target;
    const dropSquareIndex = parseInt(dropSquarePiece.parentNode.id);

    if (dropSquarePiece.getAttribute("data-color") !== draggedPiece.getAttribute("data-color")){

        dropSquarePiece.remove();
        movePiece(draggedPieceStartIndex, dropSquareIndex);
        recordMove(draggedPieceStartIndex, dropSquareIndex);

    }

}

function handleDragEnd(){

    draggedPiece = null;
    draggedPieceStartIndex = null;
    
}

function recordMove(fromSquareIndex, toSquareIndex){

    const piece = draggedPiece;
    const pieceType = piece.getAttribute("data-type");
    const fromSquareName = indexToSquareName(fromSquareIndex);
    const toSquareName = indexToSquareName(toSquareIndex);
    const move = `${fromSquareName}${toSquareName}`;

    if(halfMove % 2 == 0){
        let moveIndex = 1 + halfMove / 2;
        let moveIndexStr = moveIndex.toString();
        pgnStr += moveIndexStr + '. ';
    }
    
    pgnStr += `${move} `;
    halfMove++;

    document.getElementById("pgnOutput").value = pgnStr;
}

function indexToSquareName(index){

    const file = index % 8;
    const rank = Math.floor(index / 8);

    const fileNames = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
    return `${fileNames[file]}${8 - rank}`;

}

function movePiece(fromSquareIndex, toSquareIndex){

    const fromSquareElement = document.getElementById(fromSquareIndex);
    const toSquareElement = document.getElementById(toSquareIndex);
    const piece = fromSquareElement.querySelector('.piece');
    toSquareElement.appendChild(piece);

}

