import 'dart:convert';
import 'package:http/http.dart' as http;

import 'package:can_to_go_gui/data/models/can_message_model.dart';

class EspRestApiRemoteDataSource {
  final String _url = "http://192.168.4.1/can";

  Future<List<CanMessageModel>> getCanMessages() async {
    final http.Response response = await http.get(Uri.parse(_url));

    List<dynamic> canMessagesJson = json.decode(response.body)["messages"];

    return canMessagesJson
        .map((messageJson) => CanMessageModel.fromJson(messageJson))
        .toList();
  }
}
