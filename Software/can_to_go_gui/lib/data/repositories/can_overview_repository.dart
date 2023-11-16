import 'package:can_to_go_gui/data/models/can_message_model.dart';
import 'package:can_to_go_gui/data/datasources/esp_rest_api_remote_datasource.dart';

List<CanMessageModel> canMessagesCache = [];
List<CanMessageModel> canMessagesUniqueIdCache = [];

class CanOverviewRepository {
  final EspRestApiRemoteDataSource espRestApiRemoteDataSource =
      EspRestApiRemoteDataSource();

  double getCycleTimeForId(
      int id, List<CanMessageModel> currentCanModelsBatch) {
    List<int> micros = currentCanModelsBatch
        .where((model) => model.id == id) // Filter models by the targetId
        .map((model) => model.micros) // Extract micros from filtered models
        .toList(); // Convert the result to a List<int>

    // Sort the microsList in descending order
    micros.sort((a, b) => b.compareTo(a));

    // Calculate differences between consecutive micros
    List<int> differencesList = [];
    for (int i = 0; i < micros.length - 1; i++) {
      int difference = micros[i] - micros[i + 1];
      differencesList.add(difference);
    }

    if (differencesList.isEmpty) {
      return 0.0; // Avoid division by zero for an empty list
    }

    // Calculate the mean using the built-in mean function from dart:math
    double mean =
        differencesList.reduce((a, b) => a + b) / differencesList.length;

    return mean / 1000.0;
  }

  Future<List<CanMessageModel>> getCanOverviewModels() async {
    final currentCanModelsBatch =
        (await espRestApiRemoteDataSource.getCanMessages()).reversed;

    for (CanMessageModel canMessage in currentCanModelsBatch) {
      if (!canMessagesCache.contains(canMessage)) {
        canMessagesCache.insert(0, canMessage);
      }
      if (canMessagesCache.length == 1001) {
        canMessagesCache.removeLast();
      }
    }

    List<CanMessageModel> currentUniqueMessages = [];
    for (CanMessageModel canMessage in canMessagesCache) {
      bool doesMessageWithIdExistInList =
          currentUniqueMessages.any((model) => model.id == canMessage.id);
      if (!doesMessageWithIdExistInList) {
        currentUniqueMessages.add(canMessage);
      }
    }

    for (CanMessageModel canMessage in currentUniqueMessages) {
      int index = canMessagesUniqueIdCache
          .indexWhere((item) => item.id == canMessage.id);

      if (canMessage.micros != 0) {
        if (index != -1) {
          canMessage.cycleTime =
              getCycleTimeForId(canMessage.id, canMessagesCache.toList());
          canMessagesUniqueIdCache[index] = canMessage;
        } else {
          canMessagesUniqueIdCache.add(canMessage);
        }
      }
    }

    return canMessagesUniqueIdCache;
  }
}
