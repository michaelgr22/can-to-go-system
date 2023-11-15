import 'package:can_to_go_gui/data/models/can_message_model.dart';
import 'package:can_to_go_gui/data/datasources/esp_rest_api_remote_datasource.dart';

List<CanMessageModel> canMessagesUniqueIdCache = [];

class CanOverviewRepository {
  final EspRestApiRemoteDataSource espRestApiRemoteDataSource =
      EspRestApiRemoteDataSource();

  Future<List<CanMessageModel>> getCanOverviewModels() async {
    final currentCanModelsBatch =
        (await espRestApiRemoteDataSource.getCanMessages()).reversed;

    for (CanMessageModel canMessage in currentCanModelsBatch) {
      int index = canMessagesUniqueIdCache
          .indexWhere((item) => item.id == canMessage.id);

      if (index != -1) {
        canMessage.cycleTime =
            (canMessage.micros - canMessagesUniqueIdCache[index].micros) / 1000;
        canMessagesUniqueIdCache[index] = canMessage;
      } else {
        canMessagesUniqueIdCache.add(canMessage);
      }
    }

    return canMessagesUniqueIdCache;
  }
}
