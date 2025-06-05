from flask import Blueprint, render_template
from flask_login import login_required, current_user

view = Blueprint("view", __name__)

@view.route("/home")
@login_required
def home():
    return render_template("home.html", user=current_user)

@view.route("/board")
@login_required
def board():
    return render_template("board.html", user=current_user)
