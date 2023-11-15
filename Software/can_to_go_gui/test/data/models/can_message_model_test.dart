import 'package:flutter_test/flutter_test.dart';

import 'package:can_to_go_gui/data/models/can_message_model.dart';

void main() {
  test(
    'should return true because models should be considered as equal',
    () async {
      //test
      final model1 =
          CanMessageModel(micros: 20, id: 1, ext: true, dlc: 8, data: [0]);
      final model2 =
          CanMessageModel(micros: 20, id: 1, ext: false, dlc: 5, data: [1]);

      expect(true, model1 == model2);
    },
  );

  test(
    'should return false because different micros',
    () async {
      //test
      final model1 =
          CanMessageModel(micros: 20, id: 1, ext: true, dlc: 8, data: [0]);
      final model2 =
          CanMessageModel(micros: 21, id: 1, ext: false, dlc: 5, data: [1]);

      expect(false, model1 == model2);
    },
  );

  test(
    'should return false because different id',
    () async {
      //test
      final model1 =
          CanMessageModel(micros: 20, id: 1, ext: true, dlc: 8, data: [0]);
      final model2 =
          CanMessageModel(micros: 20, id: 2, ext: false, dlc: 5, data: [1]);

      expect(false, model1 == model2);
    },
  );
}
