from flask import Blueprint, render_template, request, flash, redirect, url_for
from flask_login import login_user, login_required, logout_user, current_user
from . model import User
from . import db
import hashlib
import re
import os

def validate_input(string, size_lower, size_upper):

    if len(string) < size_lower or len(string) > size_upper:
        return False
    if not re.match("^(?=.*[0-9])(?=.*[!@#$%^&*()_+-=,.<>?;:'\"\\[\\]{}|`~/])[a-zA-Z0-9!@#$%^&*()_+-=,.<>?;:'\"\\[\\]{}|`~/]+$", string):
        return False
    return True

def generate_salt(length):
    print(os.urandom(length).hex())
    return os.urandom(length).hex()

def hash_password(password):

    salt = generate_salt(16)
    salt_bytes = bytes.fromhex(salt)
    password_bytes = password.encode('utf-8')
    hash = hashlib.sha256(salt_bytes + password_bytes)
    return hash.hexdigest()

def check_password_hash(password, correct_password_hash):
    return hash_password(password) == correct_password_hash

auth = Blueprint("auth", __name__)

@auth.route("/", methods=["GET", "POST"])
@auth.route("/login", methods=["GET", "POST"])
def login():
    
    if request.method == "POST":
        form_email = request.form.get("email")
        form_password = request.form.get("password")

        existing_user = User.query.filter_by(email=form_email).first()

        if existing_user:
            if(hash_password(form_password) == existing_user.password):
                flash("Login successful!", category="success")
                login_user(existing_user, remember=True)
                return redirect(url_for("view.home"))
            else:
                flash("Wrong password!", category="error")
        else:
            flash("User with this email does not exist!", category="error")

    return render_template("login.html", user=current_user)

@auth.route("/logout")
@login_required
def logout():
    logout_user()
    return redirect(url_for("auth.login"))


@auth.route("/sign-up", methods=["GET", "POST"])
def sign_up():

    if request.method == "POST":

        form_email = request.form.get("email")
        form_username = request.form.get("username")
        form_password1 = request.form.get("password")
        form_password2 = request.form.get("password_confirm")

        existing_user = User.query.filter_by(email=form_email).first()
        
        if existing_user:
            flash("An account with that email address already exists.", category="error")
        elif len(form_email) < 5 or len(form_email) > 50:
            flash("Email must be between 5 and 50 characters long!", category="error")
        elif not validate_input(form_username, 5, 20):
            flash("Username must be between 5 and 20 characters long, use English alphabet and have at least one number and a special character!", category="error")
        elif not validate_input(form_password1, 10, 20):
            flash("Password must be between 10 and 20 characters long, use English alphabet and have at least one number and a special character!", category="error")
        elif form_password1 != form_password2:
            flash("Passwords don't match!", category="error")
        else:
            user = User(email=form_email, username=form_username, password=hash_password(form_password1))
            db.session.add(user)
            db.session.commit()
            login_user(user, remember=True)
            flash("Account created!", category="success")
            return redirect(url_for("view.home"))

    return render_template("signup.html", user=current_user)