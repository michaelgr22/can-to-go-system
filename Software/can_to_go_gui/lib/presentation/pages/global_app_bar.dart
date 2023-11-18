import 'package:flutter/material.dart';

class GlobalAppBar extends StatelessWidget {
  final List<Tab> tabs;

  const GlobalAppBar({
    super.key,
    required this.tabs,
  });

  static const String _title = 'CAN TO GO SYSTEM';

  @override
  Widget build(BuildContext context) {
    return AppBar(
      bottom: TabBar(
        tabs: tabs,
        indicatorColor: Colors.black,
        labelColor: Colors.black,
        labelStyle: const TextStyle(
          fontWeight: FontWeight.bold,
        ),
      ),
      backgroundColor: Colors.white,
      title: const Text(
        _title,
        style: TextStyle(
          color: Colors.black,
          fontWeight: FontWeight.bold,
        ),
      ),
      centerTitle: true,
    );
  }
}
