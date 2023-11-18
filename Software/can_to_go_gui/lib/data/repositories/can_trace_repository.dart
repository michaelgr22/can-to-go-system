import 'package:can_to_go_gui/data/models/can_message_model.dart';
import 'package:can_to_go_gui/data/datasources/esp_rest_api_remote_datasource.dart';

List<CanMessageModel> canMessagesTrace = [];

class CanTraceResult {
  final List<CanMessageModel> canMessages;
  final int addedCount;

  CanTraceResult(this.canMessages, this.addedCount);
}

class CanTraceRepository {
  final EspRestApiRemoteDataSource espRestApiRemoteDataSource =
      EspRestApiRemoteDataSource();

  Future<CanTraceResult> getCanTraceModels() async {
    final currentCanModelsBatch =
        await espRestApiRemoteDataSource.getCanMessages();

    int addedCount = 0;

    for (CanMessageModel canMessage in currentCanModelsBatch) {
      if (!canMessagesTrace.contains(canMessage)) {
        canMessagesTrace.insert(0, canMessage);
        addedCount++;
      }
      if (canMessagesTrace.length == 201) {
        canMessagesTrace.removeLast();
      }
    }

    return CanTraceResult(canMessagesTrace, addedCount);
  }
}
