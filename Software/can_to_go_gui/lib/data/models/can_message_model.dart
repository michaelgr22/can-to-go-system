class CanMessageModel {
  final int micros;
  final int id;
  final bool ext;
  final int dlc;
  final List<int> data;
  double? cycleTime;

  CanMessageModel({
    required this.micros,
    required this.id,
    required this.ext,
    required this.dlc,
    required this.data,
  });

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is CanMessageModel &&
          runtimeType == other.runtimeType &&
          id == other.id &&
          micros == other.micros;

  @override
  int get hashCode => id.hashCode ^ micros.hashCode;

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
