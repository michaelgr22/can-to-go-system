import 'package:flutter_test/flutter_test.dart';

import 'package:can_to_go_gui/data/datasources/esp_rest_api_remote_datasource.dart';
import 'package:can_to_go_gui/data/models/can_message_model.dart';

void main() {
  final EspRestApiRemoteDataSource espRestApiRemoteDataSource =
      EspRestApiRemoteDataSource();

  test('should return list of CanMessageModels when rest api is called',
      () async {
    //test
    final canMessages = await espRestApiRemoteDataSource.getCanMessages();

    expect(canMessages, isA<List<CanMessageModel>>());
  });
}
