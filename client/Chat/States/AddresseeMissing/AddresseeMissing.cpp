﻿#include "AddresseeMissing.h"

#include <iostream>
#include <memory>


namespace {
  //Возможный выбор пользователя
  enum {
    INPUT_AGAIN = 1,
    CANCEL
  };
}



AddresseeMissing::AddresseeMissing() : State("AddresseeMissing")
{
};



void AddresseeMissing::handle(Chat& chat)
{
  const std::string menu = "| " +
  std::to_string(INPUT_AGAIN) + " - Ввести Ник адресата повторно | " + 
  std::to_string(CANCEL) + " - Отменить отправку сообщения | : ";

  std::cout << menu;
  std::string input;
  std::getline(std::cin >> std::ws, input);

  //Введено более одного символа
  if (input.length() > 1) {
    chat.transitionTo(std::move(std::make_unique<AddresseeMissing>()));
  }
  //Введён один символ
  else {
    try {
      int choice = std::stoi(input);
      handleChoice(chat, choice);
    }
    catch (const std::invalid_argument&) {
      chat.transitionTo(std::move(std::make_unique<AddresseeMissing>()));
    }
  }
}



void AddresseeMissing::handleChoice(Chat& chat, int choice)
{
  switch (choice) {
    case INPUT_AGAIN: {
      chat.transitionTo(std::move(std::make_unique<AddresseeInput>()));
      break;
    }
    case CANCEL: {
      chat.transitionTo(std::move(std::make_unique<UserInChat>()));
      break;
    }
    default: {
      std::cin.clear();
      chat.transitionTo(std::move(std::make_unique<AddresseeMissing>()));
      break;
    }
  }
}