class CanMessageModel {
  final int micros;
  final int id;
  final bool ext;
  final int dlc;
  final List<int> data;
  int? cycleTime;

  CanMessageModel({
    required this.micros,
    required this.id,
    required this.ext,
    required this.dlc,
    required this.data,
  });

  factory CanMessageModel.fromJson(Map<String, dynamic> json) {
    return CanMessageModel(
      micros: json["micros"] as int,
      id: json["id"] as int,
      ext: json["ext"] == 1 ? true : false,
      dlc: json["dlc"] as int,
      data: List<int>.from(json["data"] as List<dynamic>),
    );
  }
}
