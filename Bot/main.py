from telegram import Update
from telegram.ext import ApplicationBuilder, CommandHandler, ContextTypes
import json
import os
from dotenv import load_dotenv
load_dotenv()

TOKEN = os.getenv('BOT_TOKEN')

database = {}

async def start(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    await update.message.reply_text("Helo, /save <Something> to save a word")

async def save(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    user_id = update.message.from_user.id
    message = " ".join(context.args)
    if user_id not in database:
        database[user_id] = []
    database[user_id].append(message)
    await update.message.reply_text(f"Data saved: {message}")

async def retrieve(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    user_id = update.message.from_user.id
    data = database.get(user_id, [])
    if data:
        await update.message.reply_text(f"Data saved: {data}")
    else:
        await update.message.reply_text("No data saved yet.")

def save_to_file():
    with open('database.json', 'w') as file:
        json.dump(database, file)

def load_from_file():
    global database
    try:
        with open('database.json', 'r') as file:
            database = json.load(file)
    except FileNotFoundError:
        database = {}

def main():
    load_from_file()
    app = ApplicationBuilder().token(TOKEN).build()

    app.add_handler(CommandHandler("start", start))
    app.add_handler(CommandHandler("save", save))
    app.add_handler(CommandHandler("retrieve", retrieve))

    app.run_polling()

    save_to_file()

if __name__ == '__main__':
    main()
