import 'package:can_to_go_gui/data/models/can_message_model.dart';
import 'package:can_to_go_gui/data/datasources/esp_rest_api_remote_datasource.dart';

List<CanMessageModel> canMessagesTrace = [];

class CanTraceRepository {
  final EspRestApiRemoteDataSource espRestApiRemoteDataSource =
      EspRestApiRemoteDataSource();

  Future<List<CanMessageModel>> getCanTraceModels() async {
    final currentCanModelsBatch =
        await espRestApiRemoteDataSource.getCanMessages();

    for (CanMessageModel canMessage in currentCanModelsBatch) {
      if (!canMessagesTrace.contains(canMessage)) {
        canMessagesTrace.insert(0, canMessage);
      }
      if (canMessagesTrace.length == 1001) {
        canMessagesTrace.removeLast();
      }
    }

    return canMessagesTrace;
  }
}
