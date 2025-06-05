function sendEmailAdmin(){

    const messageHead = 'mailto:nik.saitov.04@gmail.com?subject=Chess Website Feedback&body=';
    const messageContent = document.getElementById("feedbackOutput").value;
    const message = messageHead + messageContent;
    window.location.href = message;
    
}